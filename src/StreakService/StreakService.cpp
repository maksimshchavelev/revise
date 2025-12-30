// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <QSqlError>                       // for QSqlError
#include <QSqlQuery>                       // for QSqlQuery
#include <StreakService/StreakService.hpp> // for StreakService

namespace revise {

// public method
StreakService::StreakService(Database& driver, QObject* parent) : QObject(parent), m_db(driver) {}

// public method
int StreakService::streak() {
    QSqlQuery q(m_db.raw_db());

    if (!q.exec("SELECT value FROM streak WHERE id = 1")) {
        return 0;
    }

    if (!q.next()) {
        return 0;
    }

    return q.value("value").toInt();
}

// public method
std::expected<void, QString> StreakService::set_streak(int streak) {
    QSqlQuery q(m_db.raw_db());

    q.prepare("UPDATE streak SET value = :streak, last_updated = :now WHERE id = 1");
    q.bindValue(":streak", streak);
    q.bindValue(":now", QDateTime::currentDateTime());

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    emit streakChanged();
    return {};
}

// public method
std::expected<void, QString> StreakService::reset_if_overdue() {
    QSqlQuery q(m_db.raw_db());

    QDateTime last_update;
    QDateTime yesterday = QDateTime::currentDateTime().addDays(-1);

    QDateTime yesterday_begin = yesterday;
    QDateTime yesterday_end = yesterday;

    QDateTime today_begin = QDateTime::currentDateTime();

    yesterday_begin.setTime(QTime(0, 0, 0));
    yesterday_end.setTime(QTime(23, 59, 59));

    today_begin.setTime(QTime(0, 0, 0));

    // Get last streak updating date
    q.prepare("SELECT last_updated FROM streak WHERE id = 1");

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected("Streak table is empty");
    }

    last_update = q.value("last_updated").toDateTime();

    if ((yesterday_begin <= last_update && last_update <= yesterday_end) || (today_begin <= last_update)) {
        return {};
    }

    reset();
    return {};
}

// public method
std::expected<void, QString> StreakService::reset() {
    QSqlQuery q(m_db.raw_db());

    if (!q.exec("UPDATE streak SET value = 0 WHERE id = 1")) {
        return std::unexpected(q.lastError().text());
    }

    emit streakChanged();
    return {};
}

} // namespace revise
