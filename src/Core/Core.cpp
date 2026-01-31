// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#include <Core/Core.hpp>                               // for Core
#include <Entities/Deck.hpp>                           // for Deck
#include <ErrorReporter/ErrorReporter.hpp>             // for ErrorReporter
#include <QQmlApplicationEngine>                       // for QQmlApplicationEngine
#include <QQmlContext>                                 // for QQmlContext
#include <QSqlError>                                   // for QSqlError
#include <QUuid>                                       // for QUuid
#include <QtWebView>                                   // for QtWebView
#include <QtConcurrent>

#include <platform/PermissionServiceFactory.hpp>
#include <platform/NotificationServiceFactory.hpp>
#include <core/IPermissionService.hpp>

#ifdef Q_OS_ANDROID
// #include <QtCore/private/qandroidextras_p.h>           // for QtAndroidPrivate
#endif

#ifndef Q_OS_ANDROID
#include <QtWebEngineQuick/QtWebEngineQuick>
#endif

namespace revise {

static QString javaClassName(const QJniObject &obj)
{
    if (!obj.isValid()) return {};
    QJniEnvironment env;
    QJniObject cls = obj.callObjectMethod("getClass", "()Ljava/lang/Class;");
    if (!cls.isValid()) return {};
    QJniObject name = cls.callObjectMethod("getName", "()Ljava/lang/String;");
    if (!name.isValid()) return {};
    return name.toString();
}

// Public method
Core::Core(QGuiApplication& app, QObject* parent) :
    QObject(parent), m_app(app), m_db("revise_main", "revise.db", this), m_html_helper(this),
    m_sql_deck_repo(m_db, this), m_sm2_algo(), m_streak_service(m_db, this), m_sql_event_recorder(m_db),
    m_study_service(StudyServiceDeps{.deck_repository = m_sql_deck_repo,
                                     .algorithm = m_sm2_algo,
                                     .event_recorder = m_sql_event_recorder,
                                     .streak = m_streak_service.streak()}),
    m_anki_importer(),
    m_deck_service(
        [this]() -> std::unique_ptr<IDeckRepository> {
            const QString connName =
                QStringLiteral("repo_conn_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
            return make_thread_local_sql_repo(connName);
        },
        m_anki_importer,
        m_revise_importer,
        m_revise_exporter,
        m_deck_media_storage,
        m_html_helper,
        this),
    m_decks_model(m_deck_service), m_cards_model(m_deck_service), m_permission_service(platform::create_permission_service()),
    m_notification_service(platform::create_notification_service()){

    connect(&m_study_service, &StudyService::training_finished, this, [this]() {
        if (!m_streak_service.updated_today()) {
            m_streak_service.set_streak(m_streak_service.streak() + 1);
        }

        m_decks_model.update();
        schedule_notifications(false); // ignore today
    });

    // Run after running application
    QTimer::singleShot(0, [this]() {
        QtConcurrent::run([this](){
            if (!m_permission_service->check(core::Permission::POST_NOTIFICATIONS)) {
                qDebug() << "request post notifications";
                m_permission_service->request(core::Permission::POST_NOTIFICATIONS);
            }

            if (!m_permission_service->check(core::Permission::SCHEDULE_EXACT_ALARM)) {
                m_permission_service->request(core::Permission::SCHEDULE_EXACT_ALARM);
            }

            schedule_notifications();
        });

        // Init database manually
        if (auto init_db_res = m_db.init_db(); !init_db_res.has_value()) {
            ErrorReporter::instance()->report("Failed to init DB", init_db_res.error(), "Core::Core");
        }
    });
}


// Public method
int Core::run() {
    #ifndef Q_OS_ANDROID
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer");
        qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");

        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif

    // Copy WebView assets (html and mathjax script)
    extract_web_bundle();

    QtWebView::initialize();

    // Reset streak if overdue
    m_streak_service.reset_if_overdue();

    QQmlApplicationEngine engine;

    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("streakService", &m_streak_service);
    engine.rootContext()->setContextProperty("decksModel", &m_decks_model);
    engine.rootContext()->setContextProperty("cardsModel", &m_cards_model);
    engine.rootContext()->setContextProperty("studyService", &m_study_service);
    engine.rootContext()->setContextProperty("deckService", &m_deck_service);
    engine.rootContext()->setContextProperty("htmlHelper", &m_html_helper);
    engine.rootContext()->setContextProperty("errorReporter", ErrorReporter::instance());

    qmlRegisterUncreatableType<AppError>(
        "Revise", 1, 0, "AppError", "AppError is only available via the ErrorReporter");

    qmlRegisterUncreatableType<Deck>(
        "Revise", 1, 0, "Deck", "Deck is a value type; use DeckService to create/edit decks");

    qmlRegisterUncreatableType<Card>(
        "Revise", 1, 0, "Card", "Card is a value type; use DeckService to create/edit cards");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &m_app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("Revise", "App");

    return m_app.exec();
}



// Private method
std::unique_ptr<IDeckRepository> Core::make_thread_local_sql_repo(const QString& conn_name) {
    // create database object that opens connection with given name
    auto db = std::make_unique<Database>(conn_name, "revise.db");

    if (!db->raw_db().isOpen()) {
        qWarning() << "make_thread_local_sql_repo: Database failed to open. Conn:" << conn_name
                   << " Err:" << db->raw_db().lastError();
        return nullptr;
    }

    // OwnedRepo pattern (clearer when placed in .cpp than inside lambda)
    struct OwnedRepo : public SqlDeckRepository {
        std::unique_ptr<Database> owned_db;
        explicit OwnedRepo(std::unique_ptr<Database> db_ptr) :
            SqlDeckRepository(*db_ptr), owned_db(std::move(db_ptr)) {}
    };

    return std::make_unique<OwnedRepo>(std::move(db));
}

// Private method
void Core::extract_web_bundle() const {
    auto copy_directory_recursive = [](const QString& source_dir, const QString& destination_dir) -> bool {
        // Create a helper recursive lambda using std::function for self-reference
        std::function<bool(const QString&, const QString&)> recursive_copy;

        recursive_copy = [&recursive_copy](const QString& src, const QString& dst) -> bool {
            QDir source_dir(src);
            QDir dest_dir(dst);

            // Check if source directory exists
            if (!source_dir.exists()) {
                qWarning() << "Source directory does not exist:" << src;
                return false;
            }

            // Create destination directory if it doesn't exist
            if (!dest_dir.exists()) {
                if (!dest_dir.mkpath(".")) {
                    qWarning() << "Failed to create destination directory:" << dst;
                    return false;
                }
            }

            // Get all entries in the source directory
            QFileInfoList entries = source_dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

            for (const QFileInfo& entry : entries) {
                QString src_path = entry.absoluteFilePath();
                QString dst_path = dest_dir.absoluteFilePath(entry.fileName());

                if (entry.isDir()) {
                    // Recursively copy subdirectory
                    if (!recursive_copy(src_path, dst_path)) {
                        return false;
                    }
                } else if (entry.isFile()) {
                    // Copy file
                    if (QFile::exists(dst_path)) {
                        // Remove existing file if it exists
                        if (!QFile::remove(dst_path)) {
                            qWarning() << "Failed to remove existing file:" << dst_path;
                            return false;
                        }
                    }

                    // Copy the file
                    if (!QFile::copy(src_path, dst_path)) {
                        qWarning() << "Failed to copy file:" << src_path << "to" << dst_path;
                        return false;
                    }
                }
            }

            return true;
        };

        // Start the recursive copying process
        return recursive_copy(source_dir, destination_dir);
    };

    const auto base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/web";

    // Remove and create base dir again
    QDir(base).removeRecursively();
    QDir().mkpath(base);

    // Copy files
    copy_directory_recursive(":res/html", base);
}

// Private method
void Core::schedule_notifications(bool today_enabled, const QTime& at) const
{
    m_notification_service->clear_all_scheduled_notifications();

    for (int i = 0; i < 7; ++i) {
        if (!today_enabled && i == 0) {
            continue; // skip today
        }

        int notification_index = i;
        if (!today_enabled && i > 0) {
            notification_index = i - 1;
        }

        int       base_id = qAbs(qHash(QDateTime::currentDateTime().toMSecsSinceEpoch())) % 1000000;
        QDateTime time = QDateTime::currentDateTime().addDays(i);
        time.setTime(at);

        // If the current time is after `at`, then skip the notification
        if (QDateTime::currentDateTime() >= time)
            continue;

        m_notification_service->schedule_notification("Пора что нибудь повторить!", time);
    }
}

} // namespace revise
