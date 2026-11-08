// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Settings.hpp" // for header
#include <QDebug>       // for qWarning()

namespace ui {

Settings::Settings(core::ISettings& settings, QObject* parent) : QObject(parent), m_settings(settings) {}

// ----------------------- Algorithm settings -----------------------

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

// ----------------------- Main settings -----------------------

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


bool Settings::notify_about_early_access() const {
    return m_settings.notify_about_early_access();
}


void Settings::set_notify_about_early_access(const bool enabled) {
    if (auto res = m_settings.set_notify_about_early_access(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit notifyAboutEarlyAccessChanged();
}

// ----------------------- Dialog settings -----------------------

bool Settings::ask_remove_one_card() const {
    return m_settings.ask_remove_one_card();
}


void Settings::set_ask_remove_one_card(const bool enabled) {
    if (auto res = m_settings.set_ask_remove_one_card(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askRemoveOneCardChanged();
}


bool Settings::ask_remove_many_cards() const {
    return m_settings.ask_remove_many_cards();
}


void Settings::set_ask_remove_many_cards(const bool enabled) {
    if (auto res = m_settings.set_ask_remove_many_cards(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askRemoveManyCardsChanged();
}


bool Settings::ask_relearn_one_card() const {
    return m_settings.ask_relearn_one_card();
}


void Settings::set_ask_relearn_one_card(const bool enabled) {
    if (auto res = m_settings.set_ask_relearn_one_card(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askRelearnOneCardChanged();
}


bool Settings::ask_relearn_many_cards() const {
    return m_settings.ask_relearn_many_cards();
}


void Settings::set_ask_relearn_many_cards(const bool enabled) {
    if (auto res = m_settings.set_ask_relearn_many_cards(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askRelearnManyCardsChanged();
}


bool Settings::ask_remove_deck() const {
    return m_settings.ask_remove_deck();
}


void Settings::set_ask_remove_deck(const bool enabled) {
    if (auto res = m_settings.set_ask_remove_deck(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askRemoveDeckChanged();
}


bool Settings::ask_set_difficulty_for_many_cards() const {
    return m_settings.ask_set_difficulty_for_many_cards();
}


void Settings::set_ask_set_difficulty_for_many_cards(const bool enabled) {
    if (auto res = m_settings.set_ask_set_difficulty_for_many_cards(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askSetDifficultyForManyCardsChanged();
}


bool Settings::ask_set_state_for_many_cards() const {
    return m_settings.ask_set_state_for_many_cards();
}


void Settings::set_ask_set_state_for_many_cards(const bool enabled) {
    if (auto res = m_settings.set_ask_set_state_for_many_cards(enabled); !res) {
        qWarning() << res.error().message;
        return;
    }

    emit askSetStateForManyCardsChanged();
}

} // namespace ui
