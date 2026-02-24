// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for header
#include <QtWebView>             // for QtWebView::initialize()

#include <engine/AlgorithmFactory.hpp>     // for engine::create_study_algorithm
#include <engine/DeckService.hpp>          // for engine::DeckService
#include <engine/PopupServiceFactory.hpp>  // fpr engine::create_popup_service
#include <engine/StreakServiceFactory.hpp> // for engine::create_streak_service
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

    m_ui.init_engine(m_app);
    m_ui.bind_streak_service(*m_streak_service);
    m_ui.bind_deck_service(*m_deck_service);
    m_ui.bind_decks_model(*m_deck_service);
    m_ui.bind_cards_model(*m_deck_service);
    m_ui.bind_study_service(*m_study_service);
    m_ui.bind_popup_service(*m_popup_service);

    return m_app.exec();
}


void Launcher::init() {
    if (m_streak_storage = std::make_shared<io::SqlStreakStorage>(m_db, m_db_context); m_streak_storage) {
        qDebug() << "Streak storage created";
    }

    if (m_streak_service = engine::create_streak_service(m_streak_storage); m_streak_service) {
        qDebug() << "Streak service created";
    }

    if (m_algorithm = engine::create_study_algorithm(engine::AlgorithmType::SM2); m_algorithm) {
        qDebug() << "SM2 study algorithm created";
    }

    if (m_anki_importer = io::create_deck_importer(io::ImporterFormat::ANKI); m_anki_importer) {
        qDebug() << "Anki deck importer created";
    }

    if (m_revise_importer = io::create_deck_importer(io::ImporterFormat::REVISE); m_revise_importer) {
        qDebug() << "Revise deck importer created";
    }

    if (m_revise_exporter = io::create_deck_exporter(io::ExporterFormat::REVISE); m_revise_exporter) {
        qDebug() << "Revise deck exporter created";
    }

    if (m_deck_media_storage = io::create_deck_media_storage(); m_deck_media_storage) {
        qDebug() << "Deck media storage created";
    }

    if (m_deck_storage = std::make_unique<io::SqlDeckStorage>(m_db, m_db_context); m_deck_storage) {
        qDebug() << "Deck storage created";
    }

    engine::DeckServiceDeps deck_service_deps{.deck_storage = *m_deck_storage,
                                              .deck_media_storage = *m_deck_media_storage,
                                              .anki_deck_importer = *m_anki_importer,
                                              .revise_deck_importer = *m_revise_importer,
                                              .deck_exporter = *m_revise_exporter};

    if (m_deck_service = std::make_unique<engine::DeckService>(deck_service_deps); m_deck_service) {
        qDebug() << "Deck service created";
    }

    engine::StudyEngineDeps study_engine_deps{.algorithm = *m_algorithm, .deck_storage = *m_deck_storage};

    if (m_study_engine = std::make_unique<engine::StudyEngine>(study_engine_deps); m_study_engine) {
        qDebug() << "Study engine created";
    }

    if (m_study_service = std::make_unique<engine::StudyService>(*m_study_engine); m_study_service) {
        qDebug() << "Study service created";
    }

    if (m_popup_service = engine::create_popup_service(); m_popup_service) {
        qDebug() << "Popup service created";
    }
}


} // namespace revise