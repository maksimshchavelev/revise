// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/Settings.hpp"  // for header
#include <core/Version.hpp> // for application version

namespace io {

Settings::Settings() : m_settings("revise", "revise") {}

float Settings::learning_rate() const {
    return m_settings.value("algorithm_learning_rate", 2.5f).toFloat();
}

std::expected<void, core::SettingsError> Settings::set_learning_rate(float rate) {
    if (rate < 1.0f || rate > 5.0f) {
        return std::unexpected(core::SettingsError{.kind = core::SettingsError::Kind::OutOfBounds,
                                                   .message = QStringLiteral("rate out of bounds")});
    }

    m_settings.setValue("algorithm_learning_rate", rate);
    m_settings.sync();
    return {};
}

int Settings::max_interval() const {
    return m_settings.value("algorithm_max_interval", 14).toInt();
}

std::expected<void, core::SettingsError> Settings::set_max_interval(int interval) {
    if (interval < 1 || interval > 365) {
        return std::unexpected(core::SettingsError{.kind = core::SettingsError::Kind::OutOfBounds,
                                                   .message = QStringLiteral("rate out of bounds")});
    }

    m_settings.setValue("algorithm_max_interval", interval);
    m_settings.sync();
    return {};
}

QString Settings::application_version() const {
    return QString("%1.%2.%3")
        .arg(core::REVISE_VERSION_MAJOR)
        .arg(core::REVISE_VERSION_MINOR)
        .arg(core::REVISE_VERSION_PATCH);
}

} // namespace io
