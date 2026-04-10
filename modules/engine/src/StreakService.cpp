// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "StreakService.hpp" // for StreakService header

namespace engine {

StreakService::StreakService(std::shared_ptr<core::IStreakStorage> storage) : core::IStreakService(storage) {}


std::expected<int, QString> StreakService::get() {
    auto res = m_storage->read();

    if (!res) {
        return std::unexpected(QString("StreakService::get() failed to read streak: %1").arg(res.error()));
    }

    return res->value;
}


std::expected<void, QString> StreakService::set(int streak) {
    auto read_res = m_storage->read();

    if (!read_res) {
        return std::unexpected(QString("StreakService::set() failed to read streak: %1").arg(read_res.error()));
    }

    auto res = m_storage->save(core::Streak{.value = streak, .updated_at = read_res->updated_at});

    if (!res.has_value()) {
        return res;
    }

    dispatch(updated{streak});
    return {};
}


std::expected<void, QString> StreakService::reset_if_overdue() {
    auto overdue_res = overdue();

    if (!overdue_res) {
        return std::unexpected(overdue_res.error());
    }

    if (!overdue_res.value()) {
        return {};
    }

    auto streak = m_storage->read();

    if (!streak) {
        return std::unexpected(QString("Failed to read streak from storage: %1").arg(streak.error()));
    }

    if (streak->value == 0) {
        return {};
    }

    auto res = set(0);

    if (!res.has_value()) {
        return std::unexpected(QString("Failed to reset streak value, storage error: %1").arg(res.error()));
    }

    dispatch(reset{});

    return {};
}


std::expected<bool, QString> StreakService::overdue() const {
    QDateTime last_update;
    QDateTime yesterday = QDateTime::currentDateTime().addDays(-1);

    QDateTime yesterday_begin = yesterday;
    QDateTime yesterday_end = yesterday;

    QDateTime today_begin = QDateTime::currentDateTime();

    yesterday_begin.setTime(QTime(0, 0, 0));
    yesterday_end.setTime(QTime(23, 59, 59));

    today_begin.setTime(QTime(0, 0, 0));

    auto streak = m_storage->read();

    if (!streak.has_value()) {
        return std::unexpected(QString("Failed to get streak from storage, %1").arg(streak.error()));
    }

    last_update = streak->updated_at;

    if ((yesterday_begin <= last_update && last_update <= yesterday_end) || (today_begin <= last_update)) {
        return false;
    }

    return true;
}


std::expected<void, QString> StreakService::update() {
    auto streak = m_storage->read();

    if (!streak) {
        return std::unexpected(
            QString(" StreakService::update(): Failed to read streak from storage: %1").arg(streak.error()));
    }

    if (updated_today()) {
        return {};
    }

    auto res = m_storage->save(core::Streak{.value = streak->value + 1, .updated_at = QDateTime::currentDateTime()});

    if (!res.has_value()) {
        return res;
    }

    dispatch(updated{streak->value + 1});
    return {};
}


std::expected<bool, QString> StreakService::updated_today() const {
    auto streak = m_storage->read();

    if (!streak) {
        return std::unexpected(
            QString(" StreakService::updated_today(): Failed to read streak from storage: %1").arg(streak.error()));
    }

    QDateTime today_begin = QDateTime::currentDateTime();
    QDateTime today_end = QDateTime::currentDateTime();

    today_begin.setTime(QTime(0, 0, 0));
    today_end.setTime(QTime(23, 59, 59));

    if (today_begin <= streak->updated_at && streak->updated_at <= today_end) {
        return true;
    }

    return false;
}

} // namespace engine