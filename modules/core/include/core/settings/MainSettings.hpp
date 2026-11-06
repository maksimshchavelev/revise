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
};

} // namespace core
