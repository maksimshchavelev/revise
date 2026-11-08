// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "SettingsError.hpp" // for errors
#include <expected>          // for std::expected

namespace core {

/// @brief Confirmation Dialog Settings
class DialogSettings {
  public:
    virtual ~DialogSettings() = default;

    /// @brief Ask when user wants to remove one card?
    [[nodiscard]] virtual bool ask_remove_one_card() const = 0;

    /// @brief Specify whether to prompt the user when they delete a single card
    virtual std::expected<void, SettingsError> set_ask_remove_one_card(bool enabled) = 0;

    /// @brief Ask when user wants to remove many cards?
    [[nodiscard]] virtual bool ask_remove_many_cards() const = 0;

    /// @brief Specify whether to prompt the user when they delete many cards
    virtual std::expected<void, SettingsError> set_ask_remove_many_cards(bool enabled) = 0;

    /// @brief Ask when user wants to relearn one card?
    [[nodiscard]] virtual bool ask_relearn_one_card() const = 0;

    /// @brief Specify whether to prompt the user when they want to relearn one card
    virtual std::expected<void, SettingsError> set_ask_relearn_one_card(bool enabled) = 0;

    /// @brief Ask when user wants to relearn many cards?
    [[nodiscard]] virtual bool ask_relearn_many_cards() const = 0;

    /// @brief Specify whether to prompt the user when they want to relearn many cards
    virtual std::expected<void, SettingsError> set_ask_relearn_many_cards(bool enabled) = 0;

    /// @brief Ask when user wants to remove one deck?
    [[nodiscard]] virtual bool ask_remove_deck() const = 0;

    /// @brief Specify whether to prompt the user when they want to remove one deck
    virtual std::expected<void, SettingsError> set_ask_remove_deck(bool enabled) = 0;

    /// @brief Ask when user wants to set difficulty for many cards?
    [[nodiscard]] virtual bool ask_set_difficulty_for_many_cards() const = 0;

    /// @brief Specify whether to prompt the user when they want to set difficulty for many cards
    virtual std::expected<void, SettingsError> set_ask_set_difficulty_for_many_cards(bool enabled) = 0;

    /// @brief Ask when user wants to set state for many cards?
    [[nodiscard]] virtual bool ask_set_state_for_many_cards() const = 0;

    /// @brief Specify whether to prompt the user when they want to set state for many cards
    virtual std::expected<void, SettingsError> set_ask_set_state_for_many_cards(bool enabled) = 0;
};

} // namespace core
