// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/SqlStreakStorage.hpp" // for header
#include <QSqlError>               // for QSqlError
#include <QSqlQuery>               // for QSqlQuery

namespace io {

SqlStreakStorage::SqlStreakStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {
    if (auto res = create_streak_table(); !res.has_value()) {
        throw std::runtime_error(res.error().toStdString());
    }
}


std::expected<void, QString> SqlStreakStorage::save(const core::Streak streak) {
    return m_context.exec([this, &streak]() -> std::expected<void, QString> {
        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            UPDATE streak SET
                value = :value,
                last_updated = :last_updated
            WHERE id = 1
        )");

        q.bindValue(":value", streak.value);
        q.bindValue(":last_updated", streak.updated_at);

        if (!q.exec()) {
            return std::unexpected(QString("Failed to update value in 'streak' table. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        return {};
    });
}


std::expected<core::Streak, QString> SqlStreakStorage::read() {
    return m_context.exec([this]() -> std::expected<core::Streak, QString> {
        QSqlQuery q(m_db.raw_db());

        if (!q.exec("SELECT * FROM streak WHERE id = 1 LIMIT 1")) {
            return std::unexpected(QString("Failed to fetch value from 'streak' table. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        if (!q.next()) {
            return std::unexpected(QString("Failed to fetch value from 'streak' table (the database did not return "
                                           "anything). Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        return core::Streak{.value = q.value("value").toInt(), .updated_at = q.value("last_updated").toDateTime()};
    });
}


std::expected<void, QString> SqlStreakStorage::migrate() {
    return {}; // No migration is needed at this time.
}


std::expected<void, QString> SqlStreakStorage::create_streak_table() {
    return m_context.exec([this]() -> std::expected<void, QString> {
        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            CREATE TABLE IF NOT EXISTS streak (
                id INTEGER PRIMARY KEY CHECK (id = 1),
                value INTEGER DEFAULT 0,
                last_updated DATETIME DEFAULT (DATETIME('now', 'localtime', '-1 day')))
        )");

        if (!q.exec()) {
            return std::unexpected(QString("Failed to create table 'streak'. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        return {};
    });
}

} // namespace io