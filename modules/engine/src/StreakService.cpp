// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "StreakService.hpp" // for StreakService header

namespace engine {

StreakService::StreakService(std::shared_ptr<core::IStreakStorage> storage) : core::IStreakService(storage) {}


std::expected<core::Streak, QString> StreakService::get() {
    return m_storage->read();
}


std::expected<void, QString> StreakService::set(const core::Streak& streak) {
    auto res = m_storage->save(streak);

    if (!res.has_value()) {
        return res;
    }

    // success
    dispatch(updated{streak.value});
    return {};
}


std::expected<void, QString> StreakService::reset_if_overdue() {
    QDateTime last_update;
    QDateTime yesterday = QDateTime::currentDateTime().addDays(-1);

    QDateTime yesterday_begin = yesterday;
    QDateTime yesterday_end = yesterday;

    QDateTime today_begin = QDateTime::currentDateTime();

    yesterday_begin.setTime(QTime(0, 0, 0));
    yesterday_end.setTime(QTime(23, 59, 59));

    today_begin.setTime(QTime(0, 0, 0));

    auto streak = get();

    if (!streak.has_value()) {
        return std::unexpected(QString("Failed to get streak from storage, %1").arg(streak.error()));
    }

    last_update = streak->updated_at;

    if ((yesterday_begin <= last_update && last_update <= yesterday_end) || (today_begin <= last_update)) {
        return {};
    }

    // Reset
    streak->value = 0;
    if (auto res = set(streak.value()); !res.has_value()) {
        return std::unexpected(QString("Failed to reset streak value, storage error: %1").arg(res.error()));
    }

    dispatch(reset{});

    return {};
}

} // namespace engine