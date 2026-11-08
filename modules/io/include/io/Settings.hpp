// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QSettings>                   // for QSettings
#include <core/settings/ISettings.hpp> // for core::ISettings

namespace io {

class Settings : public core::ISettings {
  public:
    Settings();

    // ----------------------- Study algorithm settings -----------------------

    /// @copydoc core::ISettings::learning_rate
    [[nodiscard]] float learning_rate() const override;

    /// @copydoc core::ISettings::set_learning_rate
    std::expected<void, core::SettingsError> set_learning_rate(float rate) override;

    /// @copydoc core::ISettings::max_interval
    [[nodiscard]] int max_interval() const override;

    /// @copydoc core::ISettings::set_max_interval
    std::expected<void, core::SettingsError> set_max_interval(int interval) override;

    /// @copydoc core::ISettings::ui_scale
    [[nodiscard]] float ui_scale() const override;

    /// @copydoc core::ISettings::set_ui_scale
    std::expected<void, core::SettingsError> set_ui_scale(float scale) override;

    // ----------------------- Main settings -----------------------

    /// @copydoc core::ISettings::notify_about_early_access
    [[nodiscard]] bool notify_about_early_access() const override;

    /// @copydoc core::ISettings::set_notify_about_early_access
    std::expected<void, core::SettingsError> set_notify_about_early_access(bool enabled) override;

    // ----------------------- Dialog settings -----------------------
    /// @brief Ask when user wants to remove one card?
    [[nodiscard]] bool ask_remove_one_card() const override;

    /// @brief Specify whether to prompt the user when they delete a single card
    std::expected<void, core::SettingsError> set_ask_remove_one_card(bool enabled) override;

    /// @brief Ask when user wants to remove many cards?
    [[nodiscard]] bool ask_remove_many_cards() const override;

    /// @brief Specify whether to prompt the user when they delete many cards
    std::expected<void, core::SettingsError> set_ask_remove_many_cards(bool enabled) override;

    /// @brief Ask when user wants to relearn one card?
    [[nodiscard]] bool ask_relearn_one_card() const override;

    /// @brief Specify whether to prompt the user when they want to relearn one card
    std::expected<void, core::SettingsError> set_ask_relearn_one_card(bool enabled) override;

    /// @brief Ask when user wants to relearn many cards?
    [[nodiscard]] bool ask_relearn_many_cards() const override;

    /// @brief Specify whether to prompt the user when they want to relearn many cards
    std::expected<void, core::SettingsError> set_ask_relearn_many_cards(bool enabled) override;

    /// @brief Ask when user wants to remove one deck?
    [[nodiscard]] bool ask_remove_deck() const override;

    /// @brief Specify whether to prompt the user when they want to remove one deck
    std::expected<void, core::SettingsError> set_ask_remove_deck(bool enabled) override;

    /// @brief Ask when user wants to set difficulty for many cards?
    [[nodiscard]] bool ask_set_difficulty_for_many_cards() const override;

    /// @brief Specify whether to prompt the user when they want to set difficulty for many cards
    std::expected<void, core::SettingsError> set_ask_set_difficulty_for_many_cards(bool enabled) override;

    /// @brief Ask when user wants to set state for many cards?
    [[nodiscard]] bool ask_set_state_for_many_cards() const override;

    /// @brief Specify whether to prompt the user when they want to set state for many cards
    std::expected<void, core::SettingsError> set_ask_set_state_for_many_cards(bool enabled) override;

  private:
    QSettings m_settings;
};

} // namespace io
