// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "io/SqlEventStorage.hpp" // for SqlEventStorage header
#include <QJsonObject>            // for QJsonObject
#include <QSqlError>              // for QSqlError
#include <QSqlQuery>              // for QSqlQuery

namespace io {

SqlEventStorage::SqlEventStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {}


std::expected<void, QString> SqlEventStorage::record(const core::CardReviewEvent& event) {
    return m_context.exec([this, event]() -> std::expected<void, QString> {
        QJsonObject obj;
        obj["card_id"] = event.card_id;
        obj["global_card_id"] = event.global_card_id;
        obj["deck_id"] = event.deck_id;
        obj["global_deck_id"] = event.global_deck_id;
        obj["current_streak"] = event.current_streak;
        obj["review_duration_ms"] = static_cast<qint64>(event.review_duration.count());

        const QJsonDocument doc(obj);
        const QByteArray    json = doc.toJson(QJsonDocument::Compact);

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            INSERT INTO events (type, created_at, payload, sync_state)
            VALUES (
                :event_type,
                :created_at,
                :payload,
                :sync_state
            )
        )");

        q.bindValue(":event_type", static_cast<int>(core::EventType::CARD_REVIEW));
        q.bindValue(":created_at", QDateTime::currentDateTimeUtc());
        q.bindValue(":payload", json);
        q.bindValue(":sync_state", static_cast<int>(core::EventSyncState::NOT_SYNCED));

        if (!q.exec()) {
            return std::unexpected(QString("Failed to insert event into DB: %1").arg(q.lastError().text()));
        }

        return {};
    });
}



std::expected<QVector<QVariant>, QString> SqlEventStorage::fetch_synced(const core::EventType      event_type,
                                                                        const core::EventSyncState sync_state,
                                                                        int                        limit) {
    return m_context.exec([this, event_type, sync_state, limit]() -> std::expected<QVector<QVariant>, QString> {
        QVector<QVariant> result;

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            SELECT * FROM events WHERE
                type = :type AND sync_state = :sync
            LIMIT :limit
        )");

        q.bindValue(":type", static_cast<int>(event_type));
        q.bindValue(":sync_state", static_cast<int>(sync_state));
        q.bindValue(":limit", limit);

        if (!q.exec()) {
            return std::unexpected(QString("Failed to fetch synced cards from fetch table. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        while (q.next()) {
            QJsonObject json = q.value("payload").toJsonObject();

            core::CardReviewEvent event{.event_id = q.value("id").toInt(),
                                        .card_id = json["card_id"].toInt(),
                                        .global_card_id = json["global_card_id"].toInt(),
                                        .global_deck_id = json["global_deck_id"].toInt(),
                                        .current_streak = json["current_streak"].toInt(),
                                        .review_duration = std::chrono::milliseconds(json["review_duration"].toInt())};

            result.push_back(QVariant::fromValue(std::move(event)));
        }

        return result;
    });
}



std::expected<void, QString> SqlEventStorage::mark_synced_batch(QVector<int>               ids,
                                                                const core::EventSyncState sync_state) {
    return m_context.exec([this, &ids, sync_state]() -> std::expected<void, QString> {
        QSqlQuery q(m_db.raw_db());

        if (!m_db.begin_transaction()) {
            return std::unexpected(
                QString("Failed to begin mark synced transaction, cause: %1").arg(m_db.raw_db().lastError().text()));
        }

        for (const int id : ids) {
            q.prepare(R"(
            UPDATE events SET
                sync_state = :sync
            WHERE id = :id
        )");

            q.bindValue(":sync_state", static_cast<int>(sync_state));
            q.bindValue(":id", id);

            if (!q.exec()) {
                const auto _ = m_db.rollback_transaction();
                return std::unexpected(
                    QString("Failed to update event sync state with id = %1 in events table. Query: %2, cause: %3")
                        .arg(id)
                        .arg(q.lastQuery())
                        .arg(q.lastError().text()));
            }
        }

        if (!m_db.commit_transaction()) {
            return std::unexpected(
                QString("Failed to commit mark synced transaction, cause: %1").arg(m_db.raw_db().lastError().text()));
        }

        return {};
    });
}

} // namespace io
