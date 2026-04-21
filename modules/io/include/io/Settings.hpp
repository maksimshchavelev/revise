// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QSettings>                   // for QSettings
#include <core/settings/ISettings.hpp> // for core::ISettings

namespace io {

class Settings : public core::ISettings {
  public:
    /// @param path Path to settings file
    Settings();

    /// @copydoc core::ISettings::learning_rate
    float learning_rate() const override;

    /// @copydoc core::ISettings::set_learning_rate
    virtual std::expected<void, core::SettingsError> set_learning_rate(float rate) override;

    /// @copydoc core::ISettings::max_interval
    virtual int max_interval() const override;

    /// @copydoc core::ISettings::set_max_interval
    virtual std::expected<void, core::SettingsError> set_max_interval(int interval) override;

    /// @copydoc core::ISettings::application_version
    QString application_version() const override;

  private:
    QSettings m_settings;
};

} // namespace io
