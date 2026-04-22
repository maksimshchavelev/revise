// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "engine/StreakService.hpp" // for StreakService header
#include <QtConcurrentRun>          // for QtConcurrent::run

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


core::IStreakService::Error StreakService::from_streak_storage(const core::IStreakStorage::Error& error,
                                                               const QString&                     what) const {
    switch (error.kind) {
    case core::IStreakStorage::ErrorKind::InternalError:
    case core::IStreakStorage::ErrorKind::MissingValue:
        return Error::internal_error(QString("%1: %2").arg(what).arg(error.message));

    case core::IStreakStorage::ErrorKind::Unavailable:
        return Error::unavailable(QString("%1: %2").arg(what).arg(error.message));
    }
}


QFuture<StreakService::Result<core::Streak::value_type>> StreakService::get() const {
    return QtConcurrent::run([this]() -> StreakService::Result<int> {
        auto res = m_storage.read();

        if (!res) {
            return std::unexpected(from_streak_storage(res.error(), "StreakService::get(): failed to read streak"));
        }

        return res->value;
    });
}


QFuture<StreakService::Result<void>> StreakService::set(core::Streak::value_type streak) {
    return QtConcurrent::run([this, streak]() -> StreakService::Result<void> {
        if (streak < 0) {
            return std::unexpected(Error::invalid_value("streak must be positive or 0"));
        }

        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::set(): failed to read streak"));
        }

        auto save_res = m_storage.save(core::Streak{.value = streak, .updated_at = read_res->updated_at});

        if (!save_res) {
            return std::unexpected(
                from_streak_storage(save_res.error(), "StreakService::set(): failed to save streak"));
        }

        dispatch(updated{streak});
        return {};
    });
}


QFuture<StreakService::Result<void>> StreakService::reset_if_overdue() {
    return QtConcurrent::run([this]() -> StreakService::Result<void> {
        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::reset_if_overdue(): failed to read streak"));
        }

        if (!is_overdue(*read_res)) {
            return {};
        }

        if (read_res->value == 0) {
            return {};
        }

        read_res->value = 0;
        auto save_res = m_storage.save(*read_res);

        if (!save_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::reset_if_overdue(): failed to reset streak"));
        }

        dispatch(reset{});
        return {};
    });
}


QFuture<StreakService::Result<bool>> StreakService::overdue() const {
    return QtConcurrent::run([this]() -> StreakService::Result<bool> {
        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::overdue(): failed to read streak"));
        }

        return is_overdue(*read_res);
    });
}


QFuture<StreakService::Result<void>> StreakService::update() {
    return QtConcurrent::run([this]() -> StreakService::Result<void> {
        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::update(): failed to read streak"));
        }

        if (is_updated_today(*read_res)) {
            return {};
        }

        int  new_value = read_res->value + 1;
        auto res = m_storage.save(core::Streak{.value = new_value, .updated_at = QDateTime::currentDateTime()});

        if (!res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::update(): failed to save updated streak"));
        }

        dispatch(updated{new_value});
        return {};
    });
}


QFuture<StreakService::Result<bool>> StreakService::updated_today() const {
    return QtConcurrent::run([this]() -> StreakService::Result<bool> {
        auto read_res = m_storage.read();

        if (!read_res) {
            return std::unexpected(
                from_streak_storage(read_res.error(), "StreakService::updated_today(): failed to read streak"));
        }

        return is_updated_today(*read_res);
    });
}

} // namespace engine