// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication>       // for QGuiApplication
#include <QQmlApplicationEngine> // for QQmlApplicationEngine

#include <core/IDeckService.hpp>   // for core::IDeckService
#include <core/IStreakService.hpp> // for core::IStreakService
#include <core/IStudyService.hpp>  // for core::IStudyService

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

    /// Make StreakService accessible from qml
    void bind_streak_service(core::IStreakService& streak_service);

    /// Make DeckService accessible from qml
    void bind_deck_service(core::IDeckService& deck_service);

    /// Make DecksModel accessible from qml
    void bind_decks_model(core::IDeckService& deck_service);

    /// Make CardsModel accessible from qml
    void bind_cards_model(core::IDeckService& deck_service);

    /// Make StudyService accessible from qml
    void bind_study_service(core::IStudyService& study_service);

  private:
    QQmlApplicationEngine m_engine;
};

} // namespace ui