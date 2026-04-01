// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for header
#include <QtWebView>             // for QtWebView::initialize()

#include <engine/AlgorithmFactory.hpp>     // for engine::create_study_algorithm
#include <engine/DeckService.hpp>          // for engine::DeckService
#include <engine/PopupServiceFactory.hpp>  // for engine::create_popup_service
#include <engine/StreakServiceFactory.hpp> // for engine::create_streak_service
#include <engine/ToastServiceFactory.hpp>  // for engine:create_toast_service
#include <io/DeckExporterFactory.hpp>      // for io::create_deck_exporter
#include <io/DeckImporterFactory.hpp>      // for io::create_deck_importer
#include <io/DeckMediaStorageFactory.hpp>  // for io::create_deck_media_storage

namespace revise {

Launcher::Launcher(QGuiApplication& app) : m_app(app) {}


int Launcher::run() {
#ifndef Q_OS_ANDROID
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer");
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");
#endif

    QtWebView::initialize();

    init();

    if (auto res = m_streak_storage->migrate(); !res) {
        qWarning() << "Failed to apply streak storage migrations:" << res.error();
    }

    auto debug_bounds = qEnvironmentVariableIntegerValue("QML_DEBUG_BOUNDS");

    m_ui.bind_streak_service(*m_streak_service);
    m_ui.bind_deck_service(*m_deck_service);
    m_ui.bind_decks_model(*m_deck_service);
    m_ui.bind_cards_model(*m_deck_service);
    m_ui.bind_study_service(*m_study_service);
    m_ui.bind_popup_service(*m_popup_service);
    m_ui.bind_toast_service(*m_toast_service);
    m_ui.bind_router(m_router);
    m_ui.enable_debug_bounds(debug_bounds ? debug_bounds.value() : false);

    m_ui.init_engine(m_app);

    connect_signals();

    m_router.push_page("home", m_ui.create_page(QUrl("qrc:/qml/pages/Home.qml")));
    m_router.push_page("deckEditor", m_ui.create_page(QUrl("qrc:/qml/pages/DeckEditor.qml")));
    m_router.push_page("training", m_ui.create_page(QUrl("qrc:/qml/pages/Training.qml")));
    m_router.push_page("cardEditor", m_ui.create_page(QUrl("qrc:/qml/pages/CardEditor.qml")));
    m_router.push_page("cardPreview", m_ui.create_page(QUrl("qrc:/qml/pages/CardPreview.qml")));

    QTimer::singleShot(0, [this]() { post_launch(); });

    return m_app.exec();
}


void Launcher::init() {
    if (m_streak_storage = std::make_shared<io::SqlStreakStorage>(m_db, m_db_context); !m_streak_storage) {
        qWarning() << "Failed to create streak storage, got nullptr";
    }

    if (m_streak_service = engine::create_streak_service(m_streak_storage); !m_streak_service) {
        qWarning() << "Failed to create streak service, got nullptr";
    }

    if (m_algorithm = engine::create_study_algorithm(engine::AlgorithmType::SM2); !m_algorithm) {
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

    if (m_deck_storage = std::make_unique<io::SqlDeckStorage>(m_db, m_db_context); !m_deck_storage) {
        qWarning() << "Failed to create sql deck storage, got nullptr";
    }

    if (m_toast_service = engine::create_toast_service(); !m_toast_service) {
        qWarning() << "Failed to create toast service, got nullptr";
    }

    engine::DeckServiceDeps deck_service_deps{.deck_storage = *m_deck_storage,
                                              .deck_media_storage = *m_deck_media_storage,
                                              .anki_deck_importer = *m_anki_importer,
                                              .revise_deck_importer = *m_revise_importer,
                                              .deck_exporter = *m_revise_exporter};

    if (m_deck_service = std::make_unique<engine::DeckService>(deck_service_deps); !m_deck_service) {
        qWarning() << "Failed to create deck service, got nullptr";
    }

    engine::StudyEngineDeps study_engine_deps{.algorithm = *m_algorithm, .deck_storage = *m_deck_storage};

    if (m_study_engine = std::make_unique<engine::StudyEngine>(study_engine_deps); !m_study_engine) {
        qWarning() << "Failed to create study engine, got nullptr";
    }

    if (m_study_service = std::make_unique<engine::StudyService>(*m_study_engine); !m_study_service) {
        qWarning() << "Failed to create study service, got nullptr";
    }

    if (m_popup_service = engine::create_popup_service(); !m_popup_service) {
        qWarning() << "Failed to create popup service, got nullptr";
    }
}


void Launcher::connect_signals() {
    m_study_service->connect<core::IStudyService::training_finished>(
        [this](auto& e) { m_router.navigate("home", {}); });

    // Deck events
    m_deck_service->connect<core::IDeckService::decks_updated>([this](auto& e) {
        m_toast_service->request(
            core::Toast{.header = QCoreApplication::translate("deck events", "Колода обновлена"),
                        .message = QCoreApplication::translate("deck events", "Колода была обновлена"),
                        .type = core::ToastType::INFO});
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
}


void Launcher::post_launch() {
    qDebug() << "Qt launched";

    m_router.navigate(ui::Page{"home"});
}

} // namespace revise