// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Settings.hpp" // for header
#include <QDebug>       // for qWarning()

namespace ui {

Settings::Settings(core::ISettings& settings, QObject* parent) : QObject(parent), m_settings(settings) {}

int Settings::max_interval() const {
    return m_settings.max_interval();
}

void Settings::set_max_interval(int interval) {
    auto res = m_settings.set_max_interval(interval);

    if (!res) {
        qWarning() << res.error().message;
    }
}

float Settings::learning_rate() const {
    return m_settings.learning_rate();
}

void Settings::set_learning_rate(float rate) {
    auto res = m_settings.set_learning_rate(rate);

    if (!res) {
        qWarning() << res.error().message;
    }
}

} // namespace ui