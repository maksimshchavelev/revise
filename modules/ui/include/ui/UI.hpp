// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication>       // for QGuiApplication
#include <QQmlApplicationEngine> // for QQmlApplicationEngine

namespace ui {

/**
 * @brief Class that registers QML types and initializes the UI
 */
class UI {
  public:
    /**
     * @brief Constructor
     * @note To register QML types, call `UI::init_qml`; to initialize the QML engine, call `UI::init_engine`.
     */
    UI() = default;

    /**
     * @brief Registers data types in QML
     */
    void init_qml();

    /**
     * @brief Inits the QML engine
     */
    void init_engine(QGuiApplication& app);

  private:
    QQmlApplicationEngine m_engine;
};

} // namespace ui