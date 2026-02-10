// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "StreakService.hpp" // for header

namespace ui {

StreakService::StreakService(core::IStreakService& streak_service, QObject* parent) :
    QObject(parent), m_streak_service(streak_service) {

    m_streak_service.connect<core::IStreakService::reset>([this](const core::IStreakService::reset& e) {
        emit updatedTodayChanged();
        emit streakChanged();
    });

    m_streak_service.connect<core::IStreakService::updated>([this](const core::IStreakService::updated& e) {
        emit updatedTodayChanged();
        emit streakChanged();
    });

    emit streakChanged();
    emit updatedTodayChanged();
}


int StreakService::get() {
    auto res = m_streak_service.get();

    if (!res.has_value()) {
        qWarning() << "ui::StreakService::get() failed to fetch streak from core::IStreakService:" << res.error();
        return 0;
    }

    return res->value;
}


bool StreakService::updated_today() {
    auto res = m_streak_service.get();

    if (!res.has_value()) {
        qWarning() << "ui::StreakService::updated_today() failed to fetch 'updated at' date from core::IStreakService:"
                   << res.error();
        return false;
    }

    QDateTime today;

    QDateTime today_begin = today;
    QDateTime today_end = today;

    today_begin.setTime(QTime(0, 0, 0));
    today_end.setTime(QTime(23, 59, 59));

    return today_begin <= res->updated_at && res->updated_at <= today_end;
}

} // namespace ui