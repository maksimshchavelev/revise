// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication> // for QGuiApplication

#include <core/IStreakService.hpp> // for core::IStreakService
#include <core/IStreakStorage.hpp> // for core::IStreakStorage

#include "ui/UI.hpp" // for ui

namespace revise {

class Launcher {
  public:
    Launcher(QGuiApplication& app);

    int run();

  private:
    QGuiApplication& m_app;

    std::unique_ptr<core::IStreakStorage> m_streak_storage;
    std::unique_ptr<core::IStreakService> m_streak_service;

    ui::UI m_ui;
};

} // namespace revise