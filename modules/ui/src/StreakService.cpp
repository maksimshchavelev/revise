// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "StreakService.hpp" // for header
#include <QFutureWatcher>    // for QFutureWatcher

namespace ui {

StreakService::StreakService(core::IStreakService& streak_service, QObject* parent) :
    QObject(parent), m_streak_service(streak_service) {

    m_streak_service.connect<core::IStreakService::reset>([this](const core::IStreakService::reset& e) {
        QMetaObject::invokeMethod(this, [this](){ load(); });
    });

    m_streak_service.connect<core::IStreakService::updated>([this](const core::IStreakService::updated& e) {
        QMetaObject::invokeMethod(this, [this](){ load(); });
    });

    load();
}


int StreakService::streak() const noexcept {
    return m_streak;
}


bool StreakService::updated_today() const noexcept {
    return m_updated_today;
}


void StreakService::load() {
    auto streak = m_streak_service.get();
    auto updated_today = m_streak_service.updated_today();

    auto streak_watcher = new QFutureWatcher<decltype(streak.result())>(this);
    auto updated_today_watcher = new QFutureWatcher<decltype(updated_today.result())>(this);

    connect(streak_watcher, &QFutureWatcherBase::finished, this, [this, streak_watcher]() {
        auto res = streak_watcher->result();

        if (!res) {
            qWarning() << res.error().message;
            return;
        }

        m_streak = *res;
        emit streakChanged();
        streak_watcher->deleteLater();
    });

    connect(updated_today_watcher, &QFutureWatcherBase::finished, this, [this, updated_today_watcher]() {
        auto res = updated_today_watcher->result();

        if (!res) {
            qWarning() << res.error().message;
            return;
        }

        m_updated_today = *res;
        emit updatedTodayChanged();
        updated_today_watcher->deleteLater();
    });

    streak_watcher->setFuture(streak);
    updated_today_watcher->setFuture(updated_today);
}

} // namespace ui