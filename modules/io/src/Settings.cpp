// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/Settings.hpp" // for header
#include <QScreen>         // for QScreen
#include <QSize>           // for QSize

namespace io {

Settings::Settings() : m_settings("revise", "revise") {}


float Settings::learning_rate() const {
    return m_settings.value("algorithm_learning_rate", 2.5f).toFloat();
}


std::expected<void, core::SettingsError> Settings::set_learning_rate(float rate) {
    if (rate < 1.0f || rate > 5.0f) {
        return std::unexpected(
            core::SettingsError{.kind = core::SettingsError::Kind::OutOfBounds, .message = QStringLiteral("rate out of bounds")});
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
        return std::unexpected(
            core::SettingsError{.kind = core::SettingsError::Kind::OutOfBounds, .message = QStringLiteral("rate out of bounds")});
    }

    m_settings.setValue("algorithm_max_interval", interval);
    m_settings.sync();
    return {};
}


float Settings::ui_scale() const {
    return m_settings.value("main_ui_scale", 1.0f).toFloat();
}


std::expected<void, core::SettingsError> Settings::set_ui_scale(const float scale) {
    if (scale < 0.7f || scale > 1.3f) {
        return std::unexpected(core::SettingsError{.kind = core::SettingsError::Kind::OutOfBounds,
                                                   .message = QStringLiteral("UI scale out of bounds")});
    }

    m_settings.setValue("main_ui_scale", scale);
    m_settings.sync();
    return {};
}


bool Settings::notify_about_early_access() const {
    return m_settings.value("main_notify_about_early_access", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_notify_about_early_access(const bool enabled) {
    m_settings.setValue("main_notify_about_early_access", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_remove_one_card() const {
    return m_settings.value("dialogs_ask_remove_one_card", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_remove_one_card(const bool enabled) {
    m_settings.setValue("dialogs_ask_remove_one_card", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_remove_many_cards() const {
    return m_settings.value("dialogs_ask_remove_many_cards", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_remove_many_cards(const bool enabled) {
    m_settings.setValue("dialogs_ask_remove_many_cards", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_relearn_one_card() const {
    return m_settings.value("dialogs_ask_relearn_one_card", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_relearn_one_card(const bool enabled) {
    m_settings.setValue("dialogs_ask_relearn_one_card", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_relearn_many_cards() const {
    return m_settings.value("dialogs_ask_relearn_many_cards", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_relearn_many_cards(const bool enabled) {
    m_settings.setValue("dialogs_ask_relearn_many_cards", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_remove_deck() const {
    return m_settings.value("dialogs_ask_remove_deck", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_remove_deck(const bool enabled) {
    m_settings.setValue("dialogs_ask_remove_deck", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_set_difficulty_for_many_cards() const {
    return m_settings.value("dialogs_ask_set_difficulty_for_many_cards", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_set_difficulty_for_many_cards(const bool enabled) {
    m_settings.setValue("dialogs_ask_set_difficulty_for_many_cards", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_set_state_for_many_cards() const {
    return m_settings.value("dialogs_ask_set_state_for_many_cards", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_set_state_for_many_cards(const bool enabled) {
    m_settings.setValue("dialogs_ask_set_state_for_many_cards", enabled);
    m_settings.sync();
    return {};
}


bool Settings::ask_abort_learning() const {
    return m_settings.value("dialogs_ask_abort_learning", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_ask_abort_learning(bool enabled) {
    m_settings.setValue("dialogs_ask_abort_learning", enabled);
    m_settings.sync();
    return {};
}


bool Settings::notify_about_too_old_android() const {
    return m_settings.value("main_notify_about_too_old_android", true).toBool();
}


std::expected<void, core::SettingsError> Settings::set_notify_about_too_old_android(const bool enabled) {
    m_settings.setValue("main_notify_about_too_old_android", enabled);
    m_settings.sync();
    return {};
}


bool Settings::use_mobile_ui() const {
    if (m_settings.contains("main_use_mobile_ui")) {
        return m_settings.value("main_use_mobile_ui").toBool();
    }

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
#if !defined(Q_OS_ANDROID)
    return false;
#endif
#endif

    const auto* screen = QGuiApplication::primaryScreen();

    if (!screen) {
        return true;
    }

    auto phys_mm = screen->physicalSize();

    if (phys_mm.width() <= 10.0 || phys_mm.height() <= 10.0) {
        const auto native_res = screen->size() * screen->devicePixelRatio();
        const auto dpi_x = screen->physicalDotsPerInchX();
        const auto dpi_y = screen->physicalDotsPerInchY();

        if (dpi_x <= 0.0 || dpi_y <= 0.0) {
            return true;
        }

        phys_mm.setWidth((native_res.width() / dpi_x) * 25.4);
        phys_mm.setHeight((native_res.height() / dpi_y) * 25.4);
    }

    const auto min_side_mm = qMin(phys_mm.width(), phys_mm.height());
    return min_side_mm < 95.0;
}


std::expected<void, core::SettingsError> Settings::set_use_mobile_ui(const bool enabled) {
    m_settings.setValue("main_use_mobile_ui", enabled);
    m_settings.sync();
    return {};
}

} // namespace io
