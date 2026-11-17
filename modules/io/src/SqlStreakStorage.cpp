// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "io/SqlStreakStorage.hpp" // for header
#include <QSqlError>               // for QSqlError
#include <QSqlQuery>               // for QSqlQuery

namespace io {

SqlStreakStorage::SqlStreakStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {}


core::IStreakStorage::Result<void> SqlStreakStorage::save(const core::Streak streak) {
    return m_context.exec([this, &streak]() -> Result<void> {
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
    return m_context.exec([this]() -> Result<core::Streak> {
        QSqlQuery q(m_db.raw_db());

        if (!q.exec("SELECT * FROM streak WHERE id = 1 LIMIT 1")) {
            return std::unexpected(from_sql(q.lastError(), q.lastError().text()));
        }

        if (!q.next()) {
            return std::unexpected(
                Error::missing_value("There are no records in the ‘streak’ SQL table"));
        }

        return core::Streak{.value = q.value("value").toInt(), .updated_at = q.value("last_updated").toDateTime()};
    });
}


core::IStreakStorage::Error SqlStreakStorage::from_sql(const QSqlError& error, QString context) {
    switch (error.type()) {
    case QSqlError::ConnectionError:
        return Error::unavailable(std::move(context));

    default:
        return Error::internal_error(std::move(context));
    }
}

} // namespace io
