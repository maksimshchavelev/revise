// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication> // for QGuiApplication

// Streak
#include <core/IStreakService.hpp> // for core::IStreakService
#include <io/SqlStreakStorage.hpp> // for io::SqlStreakStorage

// Database
#include <io/Database.hpp>                 // for io::Database
#include <io/DatabaseExecutionContext.hpp> // for io::DatabaseExecutionContext

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

    ui::UI m_ui;

    void init();
};

} // namespace revise