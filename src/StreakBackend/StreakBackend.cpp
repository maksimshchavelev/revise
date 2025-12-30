// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Class controlling the streak

#include <QDateTime>
#include <StreakBackend/StreakBackend.hpp>

namespace revise {

// Public method
StreakBackend::StreakBackend(Database& db, QObject* parent) : QObject(parent), m_db(db) {
    auto streak_fetch_result = m_db.get_streak();

    if (!streak_fetch_result.has_value()) {
        qWarning() << streak_fetch_result.error();
        m_streak = 0;
    } else {
        m_streak = streak_fetch_result.value();
    }
}

// Public method
quint32 StreakBackend::streak() const noexcept {
    return m_streak;
}

// Public method
void StreakBackend::set_streak(quint32 streak) noexcept {
    auto query_res = m_db.set_streak(streak);
    if (!query_res.has_value()) {
        qWarning() << query_res.error();
        return;
    }

    m_streak = streak;
    emit streak_updated();
    emit is_updated_today_changed();
}

// Public method
void StreakBackend::reset_streak_if_overdue() noexcept {
    auto query_res = m_db.get_last_streak_updating_date();

    if (!query_res.has_value()) {
        qWarning() << query_res.error();
        return;
    }

    QDateTime last_update = query_res.value();
    QDateTime yesterday = QDateTime::currentDateTime().addDays(-1);

    QDateTime yesterday_begin = yesterday;
    QDateTime yesterday_end = yesterday;

    QDateTime today_begin = QDateTime::currentDateTime();

    yesterday_begin.setTime(QTime(0, 0, 0));
    yesterday_end.setTime(QTime(23, 59, 59));

    today_begin.setTime(QTime(0, 0, 0));

    // Streak updated yesterday or today
    if ((yesterday_begin <= last_update && last_update <= yesterday_end) ||
        (today_begin <= last_update)) {
        return;
    }

    // Reset streak
    reset();
}

// Public method
bool StreakBackend::is_updated_today() noexcept
{
    auto query_res = m_db.get_last_streak_updating_date();

    if (!query_res.has_value()) {
        qWarning() << query_res.error();
        return false;
    }

    QDateTime last_update = query_res.value();

    QDateTime today = QDateTime::currentDateTime();
    today.setTime(QTime(0, 0, 0));

    emit is_updated_today_changed();

    return last_update >= today;
}


// Public method
void StreakBackend::reset() noexcept
{
    auto query_res = m_db.reset_streak();
    if (!query_res.has_value()) {
        qWarning() << query_res.error();
        return;
    }

    m_streak = 0;
    emit streak_updated();
}

} // namespace revise
