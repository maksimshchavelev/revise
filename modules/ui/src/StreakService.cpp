// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "StreakService.hpp" // for header
#include <QFutureWatcher>    // for QFutureWatcher

namespace ui {

StreakService::StreakService(core::IStreakService& streak_service, QObject* parent) :
    QObject(parent), m_streak_service(streak_service) {

    m_streak_service.connect<core::IStreakService::reset>(
        [this](const core::IStreakService::reset&) { QMetaObject::invokeMethod(this, [this]() { load(); }); });

    m_streak_service.connect<core::IStreakService::updated>(
        [this](const core::IStreakService::updated&) { QMetaObject::invokeMethod(this, [this]() { load(); }); });

    load();
}


int StreakService::streak() const noexcept {
    return m_streak;
}


bool StreakService::updated_today() const noexcept {
    return m_updated_today;
}


void StreakService::load() {
    m_streak_service.get().then(this, [this](core::IStreakService::Result<core::Streak::value_type> result) {
        if (!result) {
            qWarning() << "Failed to load streak: " << result.error().message;
            return;
        }

        m_streak = *result;
        emit streakChanged();
    });

    m_streak_service.updated_today().then(this, [this](core::IStreakService::Result<bool> result) {
        if (!result) {
            qWarning() << "Failed to check that streak is updated today: " << result.error().message;
            return;
        }

        m_updated_today = *result;
        emit updatedTodayChanged();
    });
}

} // namespace ui
