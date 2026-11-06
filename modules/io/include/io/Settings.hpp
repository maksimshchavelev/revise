// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QSettings>                   // for QSettings
#include <core/settings/ISettings.hpp> // for core::ISettings

namespace io {

class Settings : public core::ISettings {
  public:
    Settings();

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
    virtual std::expected<void, core::SettingsError> set_ui_scale(float scale);


  private:
    QSettings m_settings;
};

} // namespace io
