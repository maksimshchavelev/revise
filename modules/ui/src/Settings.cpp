// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Settings.hpp" // for header
#include <QDebug>       // for qWarning()

namespace ui {

Settings::Settings(core::ISettings& settings, QObject* parent) : QObject(parent), m_settings(settings) {}

int Settings::max_interval() const {
    return m_settings.max_interval();
}


void Settings::set_max_interval(const int interval) const {
    if (auto res = m_settings.set_max_interval(interval); !res) {
        qWarning() << res.error().message;
    }
}


float Settings::learning_rate() const {
    return m_settings.learning_rate();
}


void Settings::set_learning_rate(const float rate) const {
    if (auto res = m_settings.set_learning_rate(rate); !res) {
        qWarning() << res.error().message;
    }
}

} // namespace ui
