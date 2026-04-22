// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "StreakService.hpp" // for StreakService header
#include <QtConcurrentRun>   // for QtConcurrent::run

namespace engine {

StreakService::StreakService(core::IStreakStorage& storage) : m_storage(storage) {}


bool StreakService::is_updated_today(const core::Streak& streak) const {
    QDateTime today_begin = QDate::currentDate().startOfDay();
    QDateTime today_end = QDate::currentDate().endOfDay();

    return today_begin <= streak.updated_at && streak.updated_at <= today_end;
}


bool StreakService::is_overdue(const core::Streak& streak) const {
    QDateTime last_update = streak.updated_at;
    QDate     yesterday = QDate::currentDate().addDays(-1);

    QDateTime yesterday_begin = yesterday.startOfDay();
    QDateTime yesterday_end = yesterday.endOfDay();

    QDateTime today_begin = QDate::currentDate().startOfDay();

    if ((yesterday_begin <= last_update && last_update <= yesterday_end) || (today_begin <= last_update)) {
        return false;
    }

    return true;
}


QFuture<StreakService::Result<int>> StreakService::get() const {
    return QtConcurrent::run([this]() -> StreakService::Result<int> {
        auto res = m_storage.read();

        if (!res) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::get(): failed to read streak: %1").arg(res.error().message)));
        }

        return res->value;
    });
}


QFuture<StreakService::Result<void>> StreakService::set(int streak) {
    return QtConcurrent::run([this, streak]() -> StreakService::Result<void> {
        if (streak < 0) {
            return std::unexpected(Error::invalid_value("streak must be positive or 0"));
        }

        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::set(): failed to read streak: %1").arg(read_res.error().message)));
        }

        auto res = m_storage.save(core::Streak{.value = streak, .updated_at = read_res->updated_at});

        if (!res) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::set(): failed to save streak: %1").arg(res.error().message)));
        }

        dispatch(updated{streak});
        return {};
    });
}


QFuture<StreakService::Result<void>> StreakService::reset_if_overdue() {
    return QtConcurrent::run([this]() -> StreakService::Result<void> {
        auto streak = m_storage.read();

        if (!streak) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::reset_if_overdue(): failed to read streak: %1").arg(streak.error().message)));
        }

        if (!is_overdue(*streak)) {
            return {};
        }

        if (streak->value == 0) {
            return {};
        }

        streak->value = 0;
        auto save_res = m_storage.save(*streak);

        if (!save_res) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::reset_if_overdue(): failed to set streak: %1").arg(streak.error().message)));
        }

        dispatch(reset{});
        return {};
    });
}


QFuture<StreakService::Result<bool>> StreakService::overdue() const {
    return QtConcurrent::run([this]() -> StreakService::Result<bool> {
        auto streak = m_storage.read();

        if (!streak) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::overdue(): failed to read streak: %1").arg(streak.error().message)));
        }

        return is_overdue(*streak);
    });
}


QFuture<StreakService::Result<void>> StreakService::update() {
    return QtConcurrent::run([this]() -> StreakService::Result<void> {
        auto streak = m_storage.read();

        if (!streak) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::update(): failed to read streak: %1").arg(streak.error().message)));
        }

        if (is_updated_today(*streak)) {
            return {};
        }

        int  new_value = streak->value + 1;
        auto res = m_storage.save(core::Streak{.value = new_value, .updated_at = QDateTime::currentDateTime()});

        if (!res) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::update(): failed to save streak: %1").arg(streak.error().message)));
        }

        dispatch(updated{new_value});
        return {};
    });
}


QFuture<StreakService::Result<bool>> StreakService::updated_today() const {
    return QtConcurrent::run([this]() -> StreakService::Result<bool> {
        auto streak = m_storage.read();

        if (!streak) {
            return std::unexpected(Error::internal_error(
                QString("StreakService::updated_today(): failed to read streak: %1").arg(streak.error().message)));
        }

        return is_updated_today(*streak);
    });
}

} // namespace engine