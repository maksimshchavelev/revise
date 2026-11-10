// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "SettingsError.hpp" // for errors
#include <expected>          // for std::expected

namespace core {

/// @brief Main App Settings
class MainSettings {
  public:
    virtual ~MainSettings() = default;

    /// @brief User Interface Scaling
    [[nodiscard]] virtual float ui_scale() const = 0;

    /// @brief Set User Interface Scaling
    virtual std::expected<void, SettingsError> set_ui_scale(float scale) = 0;

    /// @brief Show notification about early access
    [[nodiscard]] virtual bool notify_about_early_access() const = 0;

    /// @brief Enable or disable notification about early access
    virtual std::expected<void, SettingsError> set_notify_about_early_access(bool enabled) = 0;

    /// @brief Show notification about too old mobile
    [[nodiscard]] virtual bool notify_about_too_old_android() const = 0;

    /// @brief Enable or disable notification about too old mobile
    virtual std::expected<void, SettingsError> set_notify_about_too_old_android(bool enabled) = 0;

    /// @brief Use mobile or desktop UI?
    [[nodiscard]] virtual bool use_mobile_ui() const = 0;

    /// @brief Set usage of mobile UI
    virtual std::expected<void, SettingsError> set_use_mobile_ui(bool enabled) = 0;
};

} // namespace core
