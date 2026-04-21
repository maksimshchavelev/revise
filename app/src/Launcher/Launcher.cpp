// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for header

#include <QtConcurrent>           // for QtConcurrent
#include <QtEnvironmentVariables> // for env variables

#include <engine/AlgorithmFactory.hpp>             // for engine::create_study_algorithm
#include <engine/CardEditSessionFactory.hpp>       // for engine:create_card_edit_session
#include <engine/DeckService.hpp>                  // for engine::DeckService
#include <engine/SearchEngine.hpp>                 // for engine::SearchEngine
#include <engine/StreakServiceFactory.hpp>         // for engine::create_streak_service
#include <engine/ToastServiceFactory.hpp>          // for engine:create_toast_service
#include <io/DeckExporterFactory.hpp>              // for io::create_deck_exporter
#include <io/DeckImporterFactory.hpp>              // for io::create_deck_importer
#include <io/DeckMediaStorageFactory.hpp>          // for io::create_deck_media_storage
#include <io/Settings.hpp>                         // for io::Settings
#include <platform/NotificationServiceFactory.hpp> // for notification service factory
#include <platform/PermissionServiceFactory.hpp>   // for permission service factory

#include <platform/Platform.hpp> // for Platform tools
#include <utils/Directory.hpp>   // for directory tools

namespace revise {

Launcher::Launcher(QGuiApplication& app) : m_app(app) {}


int Launcher::run() {
    if (auto sdk_version = platform::Platform::android_sdk_version(); sdk_version) {
        qDebug() << "Running on Android with SDK version" << sdk_version.value();
    }

    init();

    bool ok{false};
    auto debug_bounds = qEnvironmentVariableIntValue("QML_DEBUG_BOUNDS", &ok);

    m_ui.bind_streak_service(*m_streak_service);
    m_ui.bind_deck_service(*m_deck_service);
    m_ui.bind_decks_model(*m_deck_service, *m_study_service);
    m_ui.bind_cards_model(*m_deck_service);
    m_ui.bind_study_service(*m_study_service);
    m_ui.bind_card_edit_session(*m_card_edit_session);
    m_ui.bind_settings(*m_settings);
    m_ui.enable_debug_bounds(ok ? debug_bounds : false);
    m_ui.bind_toast_service(*m_toast_service);
    m_ui.bind_router(m_router);
    m_ui.bind_loading_screen(m_loading_screen);

    m_ui.init_engine(m_app);

    QTimer::singleShot(0, [this]() { auto _ = QtConcurrent::run([this]() { post_launch(); }); });

    return m_app.exec();
}


void Launcher::init() {
    m_db.emplace("revise_main", global_data_dir() + "/revise.db");

    if (auto res = m_db->init_db(); !res) {
        qWarning() << "Failed to init db:" << res.error();
    }

    if (m_toast_service = engine::create_toast_service(); !m_toast_service) {
        qWarning() << "Failed to create toast service, got nullptr";
    }

    if (m_settings = std::make_unique<io::Settings>(); !m_settings) {
        qWarning() << "Failed to create settings, got nullptr";
    }

    if (m_streak_storage = std::make_shared<io::SqlStreakStorage>(*m_db, m_db_context); !m_streak_storage) {
        qWarning() << "Failed to create streak storage, got nullptr";
    }

    if (m_streak_service = engine::create_streak_service(m_streak_storage); !m_streak_service) {
        qWarning() << "Failed to create streak service, got nullptr";
    }

    if (m_algorithm = engine::create_study_algorithm(engine::AlgorithmType::SM2, *m_settings); !m_algorithm) {
        qWarning() << "Failed to create SM2 algorithm, got nullptr";
    }

    if (m_anki_importer = io::create_deck_importer(io::ImporterFormat::ANKI); !m_anki_importer) {
        qWarning() << "Failed to create anki deck importer, got nullptr";
    }

    if (m_revise_importer = io::create_deck_importer(io::ImporterFormat::REVISE); !m_revise_importer) {
        qWarning() << "Failed to create revise deck importer, got nullptr";
    }

    if (m_revise_exporter = io::create_deck_exporter(io::ExporterFormat::REVISE); !m_revise_exporter) {
        qWarning() << "Failed to create revise deck exporter, got nullptr";
    }

    if (m_deck_media_storage = io::create_deck_media_storage(); !m_deck_media_storage) {
        qWarning() << "Failed to create deck media storage, got nullptr";
    }

    if (m_deck_storage = std::make_unique<io::SqlDeckStorage>(*m_db, m_db_context); !m_deck_storage) {
        qWarning() << "Failed to create sql deck storage, got nullptr";
    }

    engine::StudyEngineDeps study_engine_deps{.algorithm = *m_algorithm, .deck_storage = *m_deck_storage};

    if (m_study_engine = std::make_unique<engine::StudyEngine>(study_engine_deps); !m_study_engine) {
        qWarning() << "Failed to create study engine, got nullptr";
    }

    if (m_search_engine = std::make_unique<engine::SearchEngine>(*m_deck_storage); !m_search_engine) {
        qWarning() << "Failed to create search engine, got nullptr";
    }

    engine::DeckServiceDeps deck_service_deps{.deck_storage = *m_deck_storage,
                                              .deck_media_storage = *m_deck_media_storage,
                                              .anki_deck_importer = *m_anki_importer,
                                              .revise_deck_importer = *m_revise_importer,
                                              .deck_exporter = *m_revise_exporter,
                                              .study_engine = *m_study_engine,
                                              .search_engine = *m_search_engine};

    if (m_deck_service = std::make_unique<engine::DeckService>(deck_service_deps); !m_deck_service) {
        qWarning() << "Failed to create deck service, got nullptr";
    }

    if (m_study_service = std::make_unique<engine::StudyService>(*m_study_engine); !m_study_service) {
        qWarning() << "Failed to create study service, got nullptr";
    }

    if (m_card_edit_session = engine::create_card_edit_session(engine::CardEditSessionType::Local);
        !m_card_edit_session) {
        qWarning() << "Failed to create card edit session, got nullptr";
    }

    if (m_permission_service = platform::create_permission_service(); !m_permission_service) {
        qWarning() << "Failed to create permission service, got nullptr";
    }

    if (m_notification_service = platform::create_notification_service(); !m_notification_service) {
        qWarning() << "Failed to create notification service, got nullptr";
    }
}


void Launcher::connect_signals() {
    m_study_service->connect<core::IStudyService::training_finished>(
        [this](auto& e) { m_router.navigate("home", {}); });

    // Deck events
    m_deck_service->connect<core::IDeckService::deck_updated>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода обновлена"),
                        .message = QCoreApplication::translate("deck events", "Колода была обновлена"),
                        .type = core::ToastType::INFO});
    });

    m_deck_service->connect<core::IDeckService::deck_created>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода создана"),
                        .message = QCoreApplication::translate("deck events", "Новая колода создана!"),
                        .type = core::ToastType::SUCCESS});
    });

    m_deck_service->connect<core::IDeckService::deck_removed>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода удалена"),
                        .message = QCoreApplication::translate("deck events", "Колода была удалена!"),
                        .type = core::ToastType::SUCCESS});
    });

    m_deck_service->connect<core::IDeckService::deck_exported>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода экспортирована"),
                        .message = QCoreApplication::translate("deck events", "Колода успешно экспортирована!"),
                        .type = core::ToastType::SUCCESS});
    });

    m_deck_service->connect<core::IDeckService::export_failed>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Экспорт не удался"),
                        .message = QCoreApplication::translate("deck events", "Причина: ") + e.error,
                        .type = core::ToastType::ERROR});
    });

    m_deck_service->connect<core::IDeckService::deck_imported>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода импортирована"),
                        .message = QCoreApplication::translate("deck events", "Колода успешно импортирована"),
                        .type = core::ToastType::SUCCESS});
    });

    m_deck_service->connect<core::IDeckService::import_failed>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Импорт не удался"),
                        .message = QCoreApplication::translate("deck events", "Причина: ") + e.error,
                        .type = core::ToastType::ERROR});
    });

    // Card events
    m_deck_service->connect<core::IDeckService::card_created>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Карточка добавлена"),
                        .message = QCoreApplication::translate("deck events", "Карточка успешно добавлена!"),
                        .type = core::ToastType::SUCCESS});
    });

    m_deck_service->connect<core::IDeckService::cards_updated>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Карточка обновлена"),
                        .message = QCoreApplication::translate("deck events", "Карточка была обновлена"),
                        .type = core::ToastType::INFO});
    });

    m_deck_service->connect<core::IDeckService::card_removed>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Карточка удалена"),
                        .message = QCoreApplication::translate("deck events", "Карточка успешно удалена!"),
                        .type = core::ToastType::SUCCESS});
    });

    // Study
    m_study_service->connect<core::IStudyService::error>([this](auto& e) {
        core::Toast toast;

        toast.header = QCoreApplication::translate("study events", "Не удалось начать обучение");
        toast.message = QCoreApplication::translate("study events", "Ошибка: ") + e.message;
        toast.type = core::ToastType::ERROR;

        m_toast_service->request(std::move(toast));
        m_router.back();
    });

    m_study_service->connect<core::IStudyService::training_finished>([this](auto& e) {
        core::Toast toast;

        toast.header = QCoreApplication::translate("study events", "Обучение завершено");
        toast.message = QCoreApplication::translate("study events", "Продолжайте в том же духе!");
        toast.type = core::ToastType::SUCCESS;

        m_toast_service->request(std::move(toast));

        m_streak_service->update();

        m_router.navigate(ui::Page{"home"});
    });

    m_study_service->connect<core::IStudyService::training_aborted>([this](auto& e) {
        core::Toast toast;

        toast.header = QCoreApplication::translate("study events", "Обучение прервано");
        toast.message = QCoreApplication::translate("study events", "Отдохните и вернитесь чуть позже :)");
        toast.type = core::ToastType::INFO;

        m_toast_service->request(std::move(toast));
    });

    // Streak service
    m_streak_service->connect<core::IStreakService::reset>([this](auto& e) {
        core::Toast toast;

        toast.header = QCoreApplication::translate("streak events", "Страйк сброшен!");
        toast.message =
            QCoreApplication::translate("streak events", "Занимайтесь каждый день, чтобы увеличивать свой страйк");
        toast.type = core::ToastType::INFO;

        m_toast_service->request(std::move(toast));
    });

    // Router
    QObject::connect(&m_router, &ui::Router::pageChanged, [this]() {
        if (m_router.current_page().name == "home") {
            m_router.clear_history();
        }
    });
}


void Launcher::post_launch() {
    m_loading_screen.set_visible(true);
    m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Распаковка web bundle..."));

    extract_web_bundle();

    m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Подключение сигналов..."));

    connect_signals();

    m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Настройка страниц..."));

    m_router.push_page("home", m_ui.create_page(QUrl("qrc:/qml/pages/Home.qml")));
    m_router.push_page("settings", m_ui.create_page(QUrl("qrc:/qml/pages/Settings.qml")));
    m_router.push_page("deckEditor", m_ui.create_page(QUrl("qrc:/qml/pages/DeckEditor.qml")));
    m_router.push_page("training", m_ui.create_page(QUrl("qrc:/qml/pages/Training.qml")));
    m_router.push_page("cardEditor", m_ui.create_page(QUrl("qrc:/qml/pages/CardEditor.qml")));
    m_router.push_page("cardPreview", m_ui.create_page(QUrl("qrc:/qml/pages/CardPreview.qml")));
    m_router.push_page("createDeck", m_ui.create_page(QUrl("qrc:/qml/pages/CreateDeck.qml")));

    m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Настройка базы данных..."));

    if (auto res = m_streak_storage->migrate(); !res) {
        qWarning() << "Failed to apply streak storage migrations:" << res.error();
    }

    if (auto res = m_streak_service->reset_if_overdue(); !res) {
        qWarning() << "Failed to reset streak:" << res.error();
    }

#ifdef Q_OS_ANDROID
    m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Проверка Android разрешений..."));
#endif

    auto android_sdk_version = platform::Platform::android_sdk_version();

    if (android_sdk_version && android_sdk_version.value() <= 31 /* android 12 */) {
        m_toast_service->request(core::Toast{
            .header = QCoreApplication::translate("launch events", "Слишком старый Android"),
            .message = QString(QCoreApplication::translate("application events",
                                                           "Похоже, вы используете Android 12 или ниже. Приложение "
                                                           "может работать некорректно. Уведомления отключены.")),
            .type = core::ToastType::INFO});
    }

    if (android_sdk_version && android_sdk_version.value() > 31 /* android 13+ */) {
        if (!m_permission_service->check(core::Permission::POST_NOTIFICATIONS)) {
            qDebug() << "Requesting POST_NOTIFICATIONS";
            m_permission_service->request(core::Permission::POST_NOTIFICATIONS);
        }

        if (!m_permission_service->check(core::Permission::SCHEDULE_EXACT_ALARM)) {
            qDebug() << "Requesting SCHEDULE_EXACT_ALARM";
            m_permission_service->request(core::Permission::SCHEDULE_EXACT_ALARM);
        }

#ifdef Q_OS_ANDROID
        m_loading_screen.set_description(QCoreApplication::translate("loading screen", "Планирование уведомлений..."));
        schedule_notifications();
#endif
    }

    m_router.navigate(ui::Page{"home"});

    m_loading_screen.set_visible(false);
}


void Launcher::extract_web_bundle() {
    auto copy_res = utils::Directory::copy_recursively(
        ":/res/html", QString("%1/web").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)));

    if (!copy_res) {
        m_toast_service->request(core::Toast{
            .header = QCoreApplication::translate("application events", "Ошибка распаковки ресурсов"),
            .message = QString(QCoreApplication::translate(
                                   "application events",
                                   "Не удалось распаковать web bundle: %1. Приложение может работать некорректно!"))
                           .arg(copy_res.error().description),
            .type = core::ToastType::ERROR});
    }
}


void Launcher::schedule_notifications() {
    m_notification_service->clear_all_scheduled_notifications();

    const auto now = QDateTime::currentDateTime();

    QDateTime base = now;
    base.setTime(QTime(10, 30, 0));

    for (int i = 0; i < 14; ++i) {
        const auto when = base.addDays(i);

        if (when <= now) {
            continue;
        }

        m_notification_service->schedule_notification(
            QCoreApplication::translate("notification", "Пора что-нибудь повторить!"), when);
    }
}


QString Launcher::global_data_dir() {
#ifdef Q_OS_LINUX
    QString base = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if (base.isEmpty()) {
        base = QDir::homePath() + "/.local/share";
    }

    const QString app_dir = base + "/revise";
    QDir().mkpath(app_dir);
    return app_dir;
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
}

} // namespace revise
