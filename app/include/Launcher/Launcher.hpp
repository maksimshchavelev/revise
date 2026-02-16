// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication> // for QGuiApplication

// Database
#include <io/Database.hpp>                 // for io::Database
#include <io/DatabaseExecutionContext.hpp> // for io::DatabaseExecutionContext

// Streak
#include <core/IStreakService.hpp> // for core::IStreakService
#include <io/SqlStreakStorage.hpp> // for io::SqlStreakStorage

// Algorithms
#include <core/IAlgorithm.hpp> // for core::IAlgorithm

// Importers / Exporters
#include <core/IDeckExporter.hpp> // for core::IDeckExporter
#include <core/IDeckImporter.hpp> // for core::IDeckImporter

// Deck storages
#include <core/IDeckMediaStorage.hpp> // for core::IDeckMediaStorage
#include <io/SqlDeckStorage.hpp>      // for io::SqlDeckStorage

// Deck service
#include <core/IDeckService.hpp> // for core::IDeckService

// Study service and engine
#include <engine/StudyEngine.hpp>  // for engine::StudyEngine
#include <engine/StudyService.hpp> // for engine::StudyService

// UI
#include "ui/UI.hpp" // for ui

namespace revise {

class Launcher {
  public:
    Launcher(QGuiApplication& app);

    int run();

  private:
    QGuiApplication& m_app;

    io::Database                 m_db;         ///< Database
    io::DatabaseExecutionContext m_db_context; ///< Database execution context

    std::shared_ptr<io::SqlStreakStorage> m_streak_storage; ///< Streak storage
    std::unique_ptr<core::IStreakService> m_streak_service; ///< Streak service

    std::unique_ptr<core::IAlgorithm> m_algorithm; /// Study algorithm

    std::unique_ptr<core::IDeckImporter> m_anki_importer;   ///< Anki deck importer
    std::unique_ptr<core::IDeckImporter> m_revise_importer; ///< Revise deck importer
    std::unique_ptr<core::IDeckExporter> m_revise_exporter; ///< Revise deck exporter

    std::unique_ptr<core::IDeckMediaStorage> m_deck_media_storage; ///< Deck media storage
    std::unique_ptr<io::SqlDeckStorage>      m_deck_storage;       ///< Deck storage

    std::unique_ptr<core::IDeckService> m_deck_service; ///< Deck service

    std::unique_ptr<engine::StudyEngine>  m_study_engine;  ///< Study engine
    std::unique_ptr<engine::StudyService> m_study_service; ///< Study service

    ui::UI m_ui;

    void init();
};

} // namespace revise