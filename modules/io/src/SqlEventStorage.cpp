// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/SqlEventStorage.hpp" // for SqlEventStorage header
#include <QJsonObject>            // for QJsonObject
#include <QSqlError>              // for QSqlError
#include <QSqlQuery>              // for QSqlQuery
#include <core/Version.hpp>       // for db schema version

namespace io {

SqlEventStorage::SqlEventStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {
    // Fill migration chain
    m_migration_chain.append(
        MigrationNode{.target_version = 2, .migrate = [this]() { return this->migrate_v1_to_v2(); }});

    // create table
    if (auto res = create_events_table(); !res.has_value()) {
        throw std::runtime_error(res.error().toStdString());
    }
}


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
                m_db.rollback_transaction();
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


std::expected<void, QString> SqlEventStorage::create_events_table() {
    return m_context.exec([this]() -> std::expected<void, QString> {
        QSqlQuery q(m_db.raw_db());

        // Create events table
        q.prepare(R"(
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY,
                type INTEGER DEFAULT 0,        -- type of event (for example, see core::EventType)
                created_at DATETIME NOT NULL,
                payload TEXT NOT NULL,         -- payload (for example, JSON description)
                sync_state INTEGER DEFAULT 0   -- synced with server?
            )
        )");

        if (!q.exec()) {
            return std::unexpected(QString("Failed to create table 'events'. Query: %1, cause: %2")
                                       .arg(q.lastQuery())
                                       .arg(q.lastError().text()));
        }

        return {};
    });
}


std::expected<void, QString> SqlEventStorage::migrate() {
    return m_context.exec([this]() -> std::expected<void, QString> {
        if (!m_db.begin_transaction()) {
            return std::unexpected(QString("Failed to begin migrate transaction (from v1 to v2), cause: %1")
                                       .arg(m_db.raw_db().lastError().text()));
        }

        for (const auto& migrate_node : m_migration_chain) {
            auto db_version = m_db.get_db_schema_version();

            if (!db_version.has_value()) {
                m_db.rollback_transaction();
                return std::unexpected(
                    QString("Failed to fetch db schema version, cause: %1").arg(m_db.raw_db().lastError().text()));
            }

            if (migrate_node.target_version > db_version.value()) {
                auto res = migrate_node.migrate();

                if (!res.has_value()) {
                    m_db.rollback_transaction();
                    return std::unexpected(res.error());
                }

                // success migration, continue
            }
        }

        if (!m_db.commit_transaction()) {
            return std::unexpected(QString("Failed to commit migrate transaction (from v1 to v2), cause: %1")
                                       .arg(m_db.raw_db().lastError().text()));
        }

        return {};
    });
}


std::expected<void, QString> SqlEventStorage::migrate_v1_to_v2() {
    return m_context.exec([this]() -> std::expected<void, QString> {
        QSqlQuery q(m_db.raw_db());

        // 1. Create new table with correct schema
        if (!q.exec(R"(
            CREATE TABLE events_new (
                id INTEGER PRIMARY KEY,
                type INTEGER NOT NULL,
                created_at DATETIME NOT NULL,
                payload TEXT NOT NULL,
                sync_state INTEGER NOT NULL
            )
        )")) {
            return std::unexpected(q.lastError().text());
        }

        if (!q.exec(R"(
            INSERT INTO events_new (id, type, created_at, payload, sync_state)
            SELECT
                id,
                CASE type
                    WHEN 'card_review' THEN 1
                    ELSE 0
                END AS type,
                created_at,
                payload,
                0 AS sync_state
            FROM events
        )")) {
            return std::unexpected(q.lastError().text());
        }

        // 3. Replace old table
        if (!q.exec("DROP TABLE events")) {
            return std::unexpected(q.lastError().text());
        }

        if (!q.exec("ALTER TABLE events_new RENAME TO events")) {
            return std::unexpected(q.lastError().text());
        }

        // 4. Create indexes
        if (!q.exec("CREATE INDEX idx_events_created_at ON events(created_at)")) {
            return std::unexpected(
                QString("Failed to create indexes for field 'created_at' for table 'events'. Query: %1, cause: %2")
                    .arg(q.lastQuery())
                    .arg(q.lastError().text()));
        }

        if (!q.exec("CREATE INDEX idx_events_sync_state ON events(sync_state)")) {
            return std::unexpected(
                QString("Failed to create indexes for field 'sync_state' for table 'events'. Query: %1, cause: %2")
                    .arg(q.lastQuery())
                    .arg(q.lastError().text()));
        }

        return {};
    });
}

} // namespace io