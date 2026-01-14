// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <QJsonDocument>                         // for QJsonDocument
#include <QJsonObject>                           // for QJsonObject
#include <QSqlError>                             // for QSqlError
#include <QSqlQuery>                             // for QSqlQuery
#include <SqlEventRecorder/SqlEventRecorder.hpp> // for SqlEventRecorder

namespace revise {

SqlEventRecorder::SqlEventRecorder(Database& db) : m_db(db) {}

std::expected<void, QString> SqlEventRecorder::record(const CardReviewEvent& event) {
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
        INSERT INTO events (type, created_at, payload, synced)
        VALUES (
            'card_review',
            :created_at,
            :payload,
            0
        )
    )");

    q.bindValue(":created_at", QDateTime::currentDateTimeUtc());
    q.bindValue(":payload", json);

    if (!q.exec()) {
        return std::unexpected(QString("Failed to insert event into DB: %1").arg(q.lastError().text()));
    }

    return {};
}

} // namespace revise
