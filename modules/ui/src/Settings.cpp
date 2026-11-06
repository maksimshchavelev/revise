// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Settings.hpp" // for header
#include <QDebug>       // for qWarning()

namespace ui {

Settings::Settings(core::ISettings& settings, QObject* parent) : QObject(parent), m_settings(settings) {}


int Settings::max_interval() const {
    return m_settings.max_interval();
}


void Settings::set_max_interval(const int interval) {
    if (auto res = m_settings.set_max_interval(interval); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit maxIntervalChanged();
}


float Settings::learning_rate() const {
    return m_settings.learning_rate();
}


void Settings::set_learning_rate(const float rate) {
    if (auto res = m_settings.set_learning_rate(rate); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit learningRateChanged();
}


float Settings::ui_scale() const {
    return m_settings.ui_scale();
}


void Settings::set_ui_scale(const float scale) {
    if (auto res = m_settings.set_ui_scale(scale); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit uiScaleChanged();
}

} // namespace ui
