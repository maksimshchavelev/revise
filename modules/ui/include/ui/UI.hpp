// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QGuiApplication>       // for QGuiApplication
#include <QQmlApplicationEngine> // for QQmlApplicationEngine
#include <QQmlComponent>         // for QQmlComponent

#include <core/ICardEditSession.hpp>   // for core::ICardEditSession
#include <core/IDeckService.hpp>       // for core::IDeckService
#include <core/IPopupService.hpp>      // for core::IPopupService
#include <core/IStreakService.hpp>     // for core::IStreakService
#include <core/IStudyService.hpp>      // for core::IStudyService
#include <core/IToastService.hpp>      // for core::IToastService
#include <core/settings/ISettings.hpp> // for core::ISettings

#include "Router.hpp"    // for Router
#include "UIExport.hpp"  // for UI_EXPORT macro

namespace ui {

/**
 * @brief Class that registers QML types and initializes the UI
 */
class UI_EXPORT UI {
  public:
    /**
     * @brief Constructor
     * @note To register QML types, call `UI::init_qml`; to initialize the QML engine, call `UI::init_engine`.
     */
    UI() = default;

    /**
     * @brief Inits the QML engine
     */
    void init_engine(QGuiApplication& app);

    /// Enables debug bounds
    void enable_debug_bounds(bool enable);

    /// Make StreakService accessible from qml
    void bind_streak_service(core::IStreakService& streak_service);

    /// Make DeckService accessible from qml
    void bind_deck_service(core::IDeckService& deck_service);

    /// Make DecksModel accessible from qml
    void bind_decks_model(core::IDeckService& deck_service, core::IStudyService& study_service);

    /// Make CardsModel accessible from qml
    void bind_cards_model(core::IDeckService& deck_service);

    /// Make StudyService accessible from qml
    void bind_study_service(core::IStudyService& study_service);

    /// Make PopupService accessible from qml
    void bind_popup_service(core::IPopupService& popup_service);

    /// Make ToastService accessible from qml
    void bind_toast_service(core::IToastService& toast_service);

    /// Make CardEditSession accessible from qml
    void bind_card_edit_session(core::ICardEditSession& session);

    /// Make Settings accessible from qml
    void bind_settings(core::ISettings& settings);

    /// Make Router accessible from qml
    void bind_router(Router& router);

    /**
     * @brief Creates a page component
     * @param source `.qml` page file
     * @return Pointer to `QQmlComponent`
     */
    QQmlComponent* create_page(QUrl source);

  private:
    QQmlApplicationEngine m_engine;
};

} // namespace ui
