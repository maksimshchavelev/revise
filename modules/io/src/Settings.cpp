// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/Settings.hpp" // for header

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

} // namespace io
