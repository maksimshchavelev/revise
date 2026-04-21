// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/SqlStreakStorage.hpp" // for header
#include <QSqlError>               // for QSqlError
#include <QSqlQuery>               // for QSqlQuery

namespace io {

SqlStreakStorage::SqlStreakStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {}


core::IStreakStorage::Result<void> SqlStreakStorage::save(const core::Streak streak) {
    return m_context.exec([this, &streak]() -> core::IStreakStorage::Result<void> {
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
            return std::unexpected(from_sql(q.lastError(), q.lastError().text()));
        }

        return {};
    });
}


core::IStreakStorage::Result<core::Streak> SqlStreakStorage::read() const {
    return m_context.exec([this]() -> core::IStreakStorage::Result<core::Streak> {
        QSqlQuery q(m_db.raw_db());

        if (!q.exec("SELECT * FROM streak WHERE id = 1 LIMIT 1")) {
            return std::unexpected(from_sql(q.lastError(), q.lastError().text()));
        }

        if (!q.next()) {
            return std::unexpected(
                core::IStreakStorage::Error::missing_value("There are no records in the ‘streak’ SQL table"));
        }

        return core::Streak{.value = q.value("value").toInt(), .updated_at = q.value("last_updated").toDateTime()};
    });
}


std::expected<void, QString> SqlStreakStorage::migrate() {
    return create_streak_table();
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

        q.prepare(R"(
            INSERT OR IGNORE INTO streak (id, value, last_updated)
            VALUES (1, 0, DATETIME('now', 'localtime', '-1 day'));
        )");

        if (!q.exec()) {
            return std::unexpected(QString("Failed to insert streak into 'streak' table. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        return {};
    });
}


core::IStreakStorage::Error SqlStreakStorage::from_sql(const QSqlError& error, QString context) {
    switch (error.type()) {
    case QSqlError::ConnectionError:
        return Error::unavailable(std::move(context));

    case QSqlError::StatementError:
    case QSqlError::TransactionError:
        return Error::internal_error(std::move(context));

    default:
        return Error::internal_error(std::move(context));
    }
}

} // namespace io