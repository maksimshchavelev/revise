// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString> // for QString

namespace core {

/// @brief License settings (whether it has been accepted or displayed), as well as the license text itself
class LicenseSettings {
  public:
    LicenseSettings() = default;
    virtual ~LicenseSettings() = default;

    /// @brief Has the license already been shown at first launch, or not?
    [[nodiscard]] virtual bool license_shown() const = 0;

    /// @brief Specify whether the license is displayed on first launch or not yet
    virtual void set_license_shown(bool shown) = 0;

    /// @brief View the license text
    [[nodiscard]] virtual QString license_text() const = 0;
};

} // namespace core
