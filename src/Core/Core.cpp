// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#include <Core/Core.hpp>                               // for Core
#include <Entities/Deck.hpp>                           // for Deck
#include <ErrorReporter/ErrorReporter.hpp>             // for ErrorReporter
#include <NotificationManager/NotificationManager.hpp> // for NotificationManager
#include <QQmlApplicationEngine>                       // for QQmlApplicationEngine
#include <QQmlContext>                                 // for QQmlContext
#include <QSqlError>                                   // for QSqlError
#include <QUuid>                                       // for QUuid
#include <QtCore/private/qandroidextras_p.h>           // for QtAndroidPrivate

namespace revise {

// Public method
Core::Core(QGuiApplication& app, QObject* parent) :
    QObject(parent), m_app(app), m_db("revise_main", "revise.db", this), m_html_helper(this),
    m_sql_deck_repo(m_db, this), m_sm2_algo(), m_streak_service(m_db, this),
    m_study_service(m_sql_deck_repo, m_sm2_algo), m_anki_importer(),
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
    m_decks_model(m_deck_service), m_cards_model(m_deck_service) {

    connect(&m_study_service, &StudyService::training_finished, this, [this]() {
        if (!m_streak_service.updated_today()) {
            m_streak_service.set_streak(m_streak_service.streak() + 1);
        }

        m_decks_model.update();
    });

    request_permission_if_not_granted("POST_NOTIFICATIONS");
    request_permission_if_not_granted("WRITE_EXTERNAL_STORAGE");
    request_permission_if_not_granted("READ_EXTERNAL_STORAGE");
    request_permission_if_not_granted("MANAGE_EXTERNAL_STORAGE");

    QVector<QString> notifications = {"Пора что нибудь повторить!",
                                      "Ты целых два дня ничего не повторял",
                                      "Ты так долго ничего не повторял...",
                                      "Найдешь 5 минут для повторения?",
                                      "Давай повторим карточки!",
                                      "Нужно повторить карточки",
                                      "Не хочешь кое что повторить?"};

    NotificationManager::clear_all_scheduled_notifications();

    // weekly shedule
    for (int i = 0; i < 7; ++i) {
        int       base_id = qAbs(qHash(QDateTime::currentDateTime().toMSecsSinceEpoch())) % 1000000;
        QDateTime time = QDateTime::currentDateTime().addDays(i);
        time.setTime(QTime(12, 00, 0)); // 12::00::00 by the local time

        // If the current time is after 12:00, then skip the notification
        if (QDateTime::currentDateTime() >= time)
            continue;

        NotificationManager::schedule_notification(notifications[i], time, base_id);
    }
}


// Public method
int Core::run() {
    // Reset streak if overdue
    m_streak_service.reset_if_overdue();

    QQmlApplicationEngine engine;

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

    engine.addImageProvider("math", &m_mathjax_renderer);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &m_app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("revise", "App");

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
void Core::request_permission_if_not_granted(const QString& permission) {
    auto permission_check_future = QtAndroidPrivate::checkPermission(QString("android.permission.%1").arg(permission));
    auto permission_check_result = permission_check_future.result(); // wait for result

    if (permission_check_result != QtAndroidPrivate::PermissionResult::Authorized) {
        // if not authorized, request permission
        [[maybe_unused]] auto permission_request_result =
            QtAndroidPrivate::requestPermission(QString("android.permission.%1").arg(permission)).result();
    }
}

} // namespace revise
