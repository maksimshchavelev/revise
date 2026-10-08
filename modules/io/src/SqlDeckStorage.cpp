// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/SqlDeckStorage.hpp" // for SqlDeckStorage
#include "SqlCodes.hpp"          // for SQL error codes
#include <QSqlError>             // for QSqlError
#include <QSqlQuery>             // for QSqlQuery
#include <QStringList>           // for QStringList

namespace io {

SqlDeckStorage::SqlDeckStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {}


core::IDeckStorage::Result<QVector<core::Deck>> SqlDeckStorage::create_decks(const QVector<core::Deck>& decks) {
    return m_context.exec([this, &decks]() -> Result<QVector<core::Deck>> {
        if (decks.isEmpty()) {
            return {};
        }

        QString query_string = R"(
            INSERT INTO decks (
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
            )
            VALUES
        )";

        QStringList values;
        values.reserve(decks.size());

        for (qsizetype i = 0; i < decks.size(); ++i) {
            values.push_back(QString("(%1, %2, %3, %4, %5, %6)")
                                 .arg(QString(":name_%1").arg(i),
                                      QString(":description_%1").arg(i),
                                      QString(":time_limit_%1").arg(i),
                                      QString(":new_limit_%1").arg(i),
                                      QString(":consolidate_limit_%1").arg(i),
                                      QString(":incorrect_limit_%1").arg(i)));
        }

        query_string += values.join(",\n");
        query_string += R"(
            RETURNING
                id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
        )";

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::create_decks(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < decks.size(); ++i) {
            const auto& deck = decks[i];

            q.bindValue(QString(":name_%1").arg(i), deck.name);
            q.bindValue(QString(":description_%1").arg(i), deck.description);
            q.bindValue(QString(":time_limit_%1").arg(i), deck.time_limit);
            q.bindValue(QString(":new_limit_%1").arg(i), deck.new_limit);
            q.bindValue(QString(":consolidate_limit_%1").arg(i), deck.review_limit);
            q.bindValue(QString(":incorrect_limit_%1").arg(i), deck.incorrect_limit);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::create_decks(): failed to insert decks");
            return std::unexpected(error);
        }

        QVector<core::Deck> created_decks;
        created_decks.reserve(decks.size());

        while (q.next()) {
            core::Deck created_deck;

            created_deck.id = q.value("id").toInt();
            created_deck.name = q.value("name").toString();
            created_deck.description = q.value("description").toString();
            created_deck.time_limit = q.value("time_limit").toInt();
            created_deck.new_limit = q.value("new_limit").toInt();
            created_deck.review_limit = q.value("consolidate_limit").toInt();
            created_deck.incorrect_limit = q.value("incorrect_limit").toInt();

            created_decks.push_back(std::move(created_deck));
        }

        return created_decks;
    });
}


core::IDeckStorage::Result<QVector<core::Deck>> SqlDeckStorage::update_decks(const QVector<core::Deck>& decks) {
    return m_context.exec([this, &decks]() -> Result<QVector<core::Deck>> {
        if (decks.isEmpty()) {
            return {};
        }

        QString query_string = R"(
            WITH input (
                id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
            ) AS (
                VALUES
        )";

        QStringList values;
        values.reserve(decks.size());

        for (qsizetype i = 0; i < decks.size(); ++i) {
            values.push_back(QString("(%1, %2, %3, %4, %5, %6, %7)")
                                 .arg(QString(":id_%1").arg(i),
                                      QString(":name_%1").arg(i),
                                      QString(":description_%1").arg(i),
                                      QString(":time_limit_%1").arg(i),
                                      QString(":new_limit_%1").arg(i),
                                      QString(":consolidate_limit_%1").arg(i),
                                      QString(":incorrect_limit_%1").arg(i)));
        }

        query_string += values.join(",\n");
        query_string += R"(
            )
            UPDATE decks
            SET
                name = (SELECT input.name
                        FROM input
                        WHERE input.id = decks.id),
                description = (SELECT input.description
                               FROM input
                               WHERE input.id = decks.id),
                time_limit = (SELECT input.time_limit
                              FROM input
                              WHERE input.id = decks.id),
                new_limit = (SELECT input.new_limit
                             FROM input
                             WHERE input.id = decks.id),
                consolidate_limit = (SELECT input.consolidate_limit
                                     FROM input
                                     WHERE input.id = decks.id),
                incorrect_limit = (SELECT input.incorrect_limit
                                   FROM input
                                   WHERE input.id = decks.id)
            WHERE id IN (SELECT id FROM input)
            RETURNING
                id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
        )";

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::update_decks(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < decks.size(); ++i) {
            const auto& deck = decks[i];

            q.bindValue(QString(":id_%1").arg(i), deck.id);
            q.bindValue(QString(":name_%1").arg(i), deck.name);
            q.bindValue(QString(":description_%1").arg(i), deck.description);
            q.bindValue(QString(":time_limit_%1").arg(i), deck.time_limit);
            q.bindValue(QString(":new_limit_%1").arg(i), deck.new_limit);
            q.bindValue(QString(":consolidate_limit_%1").arg(i), deck.review_limit);
            q.bindValue(QString(":incorrect_limit_%1").arg(i), deck.incorrect_limit);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::update_decks(): failed to update decks");
            return std::unexpected(error);
        }

        QVector<core::Deck> updated_decks;
        updated_decks.reserve(decks.size());

        while (q.next()) {
            core::Deck updated_deck;

            updated_deck.id = q.value("id").toInt();
            updated_deck.name = q.value("name").toString();
            updated_deck.description = q.value("description").toString();
            updated_deck.time_limit = q.value("time_limit").toInt();
            updated_deck.new_limit = q.value("new_limit").toInt();
            updated_deck.review_limit = q.value("consolidate_limit").toInt();
            updated_deck.incorrect_limit = q.value("incorrect_limit").toInt();

            updated_decks.push_back(std::move(updated_deck));
        }

        return updated_decks;
    });
}


core::IDeckStorage::Result<QVector<core::Deck>> SqlDeckStorage::remove_decks(const QVector<core::Deck::id_type>& ids) {
    return m_context.exec([this, &ids]() -> Result<QVector<core::Deck>> {
        if (ids.isEmpty()) {
            return {};
        }

        QStringList placeholders;
        placeholders.reserve(ids.size());

        for (qsizetype i = 0; i < ids.size(); ++i) {
            placeholders.push_back(QString(":id_%1").arg(i));
        }

        const QString query_string = QString(R"(
            DELETE FROM decks
            WHERE id IN (%1)
            RETURNING
                id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
        )")
                                         .arg(placeholders.join(", "));

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::remove_decks(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < ids.size(); ++i) {
            q.bindValue(QString(":id_%1").arg(i), ids[i]);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::remove_decks(): failed to remove decks");
            return std::unexpected(error);
        }

        QVector<core::Deck> removed_decks;
        removed_decks.reserve(ids.size());

        while (q.next()) {
            core::Deck removed_deck;

            removed_deck.id = q.value("id").toInt();
            removed_deck.name = q.value("name").toString();
            removed_deck.description = q.value("description").toString();
            removed_deck.time_limit = q.value("time_limit").toInt();
            removed_deck.new_limit = q.value("new_limit").toInt();
            removed_deck.review_limit = q.value("consolidate_limit").toInt();
            removed_deck.incorrect_limit = q.value("incorrect_limit").toInt();

            removed_decks.push_back(std::move(removed_deck));
        }

        return removed_decks;
    });
}


core::IDeckStorage::Result<QVector<core::Deck>> SqlDeckStorage::fetch_decks(const QVector<core::Deck::id_type>& ids) const {
    return m_context.exec([this, &ids]() -> Result<QVector<core::Deck>> {
        if (ids.isEmpty()) {
            return {};
        }

        QStringList placeholders;
        placeholders.reserve(ids.size());

        for (qsizetype i = 0; i < ids.size(); ++i) {
            placeholders.push_back(QString(":id_%1").arg(i));
        }

        const QString query_string = QString(R"(
            SELECT
                id,
                global_id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
            FROM decks
            WHERE id IN (%1)
        )")
                                         .arg(placeholders.join(", "));

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_decks(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < ids.size(); ++i) {
            q.bindValue(QString(":id_%1").arg(i), ids[i]);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_decks(): failed to fetch decks");
            return std::unexpected(error);
        }

        QVector<core::Deck> result;
        result.reserve(ids.size());

        while (q.next()) {
            core::Deck deck{.name = q.value("name").toString(),
                            .description = q.value("description").toString(),
                            .id = q.value("id").toInt(),
                            .global_id = q.value("global_id").toInt(),
                            .time_limit = q.value("time_limit").toInt(),
                            .new_limit = q.value("new_limit").toInt(),
                            .review_limit = q.value("consolidate_limit").toInt(),
                            .incorrect_limit = q.value("incorrect_limit").toInt()};

            result.push_back(std::move(deck));
        }

        return result;
    });
}


core::IDeckStorage::Result<QVector<core::Deck>> SqlDeckStorage::fetch_decks() const {
    return m_context.exec([this]() -> Result<QVector<core::Deck>> {
        QVector<core::Deck> result;
        QSqlQuery           q(m_db.raw_db());

        result.reserve(256);

        if (!q.prepare(R"(
            SELECT
                id,
                global_id,
                name,
                description,
                time_limit,
                new_limit,
                consolidate_limit,
                incorrect_limit
            FROM decks
        )")) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_decks(): failed to prepare query");
            return std::unexpected(error);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_decks(): failed to fetch decks");
            return std::unexpected(error);
        }

        while (q.next()) {
            core::Deck deck{.name = q.value("name").toString(),
                            .description = q.value("description").toString(),
                            .id = q.value("id").toInt(),
                            .global_id = q.value("global_id").toInt(),
                            .time_limit = q.value("time_limit").toInt(),
                            .new_limit = q.value("new_limit").toInt(),
                            .review_limit = q.value("consolidate_limit").toInt(),
                            .incorrect_limit = q.value("incorrect_limit").toInt()};

            result.push_back(std::move(deck));
        }

        return result;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::update_cards(const QVector<core::Card>& cards) {
    return m_context.exec([this, &cards]() -> Result<QVector<core::Card>> {
        if (cards.isEmpty()) {
            return {};
        }

        QString query_string = R"(
            WITH input (
                id,
                front,
                back,
                state,
                incorrect_streak,
                interval,
                difficulty,
                next_review,
                updated_at
            ) AS (
                VALUES
        )";

        QStringList values;
        values.reserve(cards.size());

        for (qsizetype i = 0; i < cards.size(); ++i) {
            values.push_back(QString("(%1, %2, %3, %4, %5, %6, %7, %8, %9)")
                                 .arg(QString(":id_%1").arg(i),
                                      QString(":front_%1").arg(i),
                                      QString(":back_%1").arg(i),
                                      QString(":state_%1").arg(i),
                                      QString(":incorrect_streak_%1").arg(i),
                                      QString(":interval_%1").arg(i),
                                      QString(":difficulty_%1").arg(i),
                                      QString(":next_review_%1").arg(i),
                                      QString(":updated_at_%1").arg(i)));
        }

        query_string += values.join(",\n");
        query_string += R"(
            )
            UPDATE cards
            SET
                front = (SELECT input.front
                         FROM input
                         WHERE input.id = cards.id),
                back = (SELECT input.back
                        FROM input
                        WHERE input.id = cards.id),
                state = (SELECT input.state
                         FROM input
                         WHERE input.id = cards.id),
                incorrect_streak = (SELECT input.incorrect_streak
                                    FROM input
                                    WHERE input.id = cards.id),
                interval = (SELECT input.interval
                            FROM input
                            WHERE input.id = cards.id),
                difficulty = (SELECT input.difficulty
                              FROM input
                              WHERE input.id = cards.id),
                next_review = (SELECT input.next_review
                               FROM input
                               WHERE input.id = cards.id),
                updated_at = (SELECT input.updated_at
                              FROM input
                              WHERE input.id = cards.id)
            WHERE id IN (SELECT id FROM input)
            RETURNING
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
        )";

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::update_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < cards.size(); ++i) {
            const auto& card = cards[i];

            q.bindValue(QString(":id_%1").arg(i), card.id);
            q.bindValue(QString(":front_%1").arg(i), card.front);
            q.bindValue(QString(":back_%1").arg(i), card.back);
            q.bindValue(QString(":state_%1").arg(i), static_cast<int>(card.state));
            q.bindValue(QString(":incorrect_streak_%1").arg(i), card.incorrect_streak);
            q.bindValue(QString(":interval_%1").arg(i), card.interval);
            q.bindValue(QString(":difficulty_%1").arg(i), card.difficulty);
            q.bindValue(QString(":next_review_%1").arg(i), card.next_review);
            q.bindValue(QString(":updated_at_%1").arg(i), card.updated_at);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::update_cards(): failed to update cards");
            return std::unexpected(error);
        }

        QVector<core::Card> updated_cards;
        updated_cards.reserve(cards.size());

        while (q.next()) {
            core::Card updated_card{.id = q.value("id").toInt(),
                                    .deck_id = q.value("deck_id").toInt(),
                                    .difficulty = q.value("difficulty").toFloat(),
                                    .state = static_cast<core::Card::State>(q.value("state").toInt()),
                                    .incorrect_streak = q.value("incorrect_streak").toInt(),
                                    .interval = q.value("interval").toInt(),
                                    .next_review = q.value("next_review").toDateTime(),
                                    .created_at = q.value("created_at").toDateTime(),
                                    .updated_at = q.value("updated_at").toDateTime(),
                                    .front = q.value("front").toString(),
                                    .back = q.value("back").toString()};

            updated_cards.push_back(std::move(updated_card));
        }

        return updated_cards;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::create_cards(const QVector<core::Card>& cards) {
    return m_context.exec([this, &cards]() -> Result<QVector<core::Card>> {
        if (cards.isEmpty()) {
            return {};
        }

        QString query_string = R"(
            INSERT INTO cards (
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
            )
            VALUES
        )";

        QStringList values;
        values.reserve(cards.size());

        for (qsizetype i = 0; i < cards.size(); ++i) {
            values.push_back(QString("(%1, %2, %3, %4, %5, %6, %7, %8, %9, %10)")
                                 .arg(QString(":deck_id_%1").arg(i),
                                      QString(":front_%1").arg(i),
                                      QString(":back_%1").arg(i),
                                      QString(":state_%1").arg(i),
                                      QString(":difficulty_%1").arg(i),
                                      QString(":interval_%1").arg(i),
                                      QString(":next_review_%1").arg(i),
                                      QString(":incorrect_streak_%1").arg(i),
                                      QString(":created_at_%1").arg(i),
                                      QString(":updated_at_%1").arg(i)));
        }

        query_string += values.join(",\n");
        query_string += R"(
            RETURNING
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
        )";

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::create_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < cards.size(); ++i) {
            const auto& card = cards[i];

            q.bindValue(QString(":deck_id_%1").arg(i), card.deck_id);
            q.bindValue(QString(":front_%1").arg(i), card.front);
            q.bindValue(QString(":back_%1").arg(i), card.back);
            q.bindValue(QString(":state_%1").arg(i), static_cast<int>(card.state));
            q.bindValue(QString(":difficulty_%1").arg(i), card.difficulty);
            q.bindValue(QString(":interval_%1").arg(i), card.interval);
            q.bindValue(QString(":next_review_%1").arg(i), card.next_review);
            q.bindValue(QString(":incorrect_streak_%1").arg(i), card.incorrect_streak);
            q.bindValue(QString(":created_at_%1").arg(i), card.created_at);
            q.bindValue(QString(":updated_at_%1").arg(i), card.updated_at);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::create_cards(): failed to insert cards");
            return std::unexpected(error);
        }

        QVector<core::Card> created_cards;
        created_cards.reserve(cards.size());

        while (q.next()) {
            core::Card created_card{.id = q.value("id").toInt(),
                                    .deck_id = q.value("deck_id").toInt(),
                                    .difficulty = q.value("difficulty").toFloat(),
                                    .state = static_cast<core::Card::State>(q.value("state").toInt()),
                                    .incorrect_streak = q.value("incorrect_streak").toInt(),
                                    .interval = q.value("interval").toInt(),
                                    .next_review = q.value("next_review").toDateTime(),
                                    .created_at = q.value("created_at").toDateTime(),
                                    .updated_at = q.value("updated_at").toDateTime(),
                                    .front = q.value("front").toString(),
                                    .back = q.value("back").toString()};

            created_cards.push_back(std::move(created_card));
        }

        return created_cards;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::fetch_cards(core::Deck::id_type deck_id) const {
    return m_context.exec([this, deck_id]() -> Result<QVector<core::Card>> {
        QVector<core::Card> result;
        QSqlQuery           q(m_db.raw_db());

        if (!q.prepare(R"(
            SELECT
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
            FROM cards
            WHERE deck_id = :deck_id
        )")) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        q.bindValue(":deck_id", deck_id);

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to fetch cards");
            return std::unexpected(error);
        }

        while (q.next()) {
            core::Card card{.id = q.value("id").toInt(),
                            .deck_id = q.value("deck_id").toInt(),
                            .difficulty = q.value("difficulty").toFloat(),
                            .state = static_cast<core::Card::State>(q.value("state").toInt()),
                            .incorrect_streak = q.value("incorrect_streak").toInt(),
                            .interval = q.value("interval").toInt(),
                            .next_review = q.value("next_review").toDateTime(),
                            .created_at = q.value("created_at").toDateTime(),
                            .updated_at = q.value("updated_at").toDateTime(),
                            .front = q.value("front").toString(),
                            .back = q.value("back").toString()};

            result.push_back(std::move(card));
        }

        return result;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::fetch_cards(const QVector<core::Card::id_type>& ids) const {
    return m_context.exec([this, &ids]() -> Result<QVector<core::Card>> {
        if (ids.isEmpty()) {
            return {};
        }

        QStringList placeholders;
        placeholders.reserve(ids.size());

        for (qsizetype i = 0; i < ids.size(); ++i) {
            placeholders.push_back(QString(":id_%1").arg(i));
        }

        const QString query_string = QString(R"(
            SELECT
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
            FROM cards
            WHERE id IN (%1)
        )")
                                         .arg(placeholders.join(", "));

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < ids.size(); ++i) {
            q.bindValue(QString(":id_%1").arg(i), ids[i]);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to fetch cards");
            return std::unexpected(error);
        }

        QVector<core::Card> result;
        result.reserve(ids.size());

        while (q.next()) {
            core::Card card{.id = q.value("id").toInt(),
                            .deck_id = q.value("deck_id").toInt(),
                            .difficulty = q.value("difficulty").toFloat(),
                            .state = static_cast<core::Card::State>(q.value("state").toInt()),
                            .incorrect_streak = q.value("incorrect_streak").toInt(),
                            .interval = q.value("interval").toInt(),
                            .next_review = q.value("next_review").toDateTime(),
                            .created_at = q.value("created_at").toDateTime(),
                            .updated_at = q.value("updated_at").toDateTime(),
                            .front = q.value("front").toString(),
                            .back = q.value("back").toString()};

            result.push_back(std::move(card));
        }

        return result;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::fetch_cards() const {
    return m_context.exec([this]() -> Result<QVector<core::Card>> {
        QVector<core::Card> result;
        QSqlQuery           q(m_db.raw_db());

        if (!q.prepare(R"(
            SELECT
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
            FROM cards
        )")) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::fetch_cards(): failed to fetch all cards");
            return std::unexpected(error);
        }

        while (q.next()) {
            core::Card card{.id = q.value("id").toInt(),
                            .deck_id = q.value("deck_id").toInt(),
                            .difficulty = q.value("difficulty").toFloat(),
                            .state = static_cast<core::Card::State>(q.value("state").toInt()),
                            .incorrect_streak = q.value("incorrect_streak").toInt(),
                            .interval = q.value("interval").toInt(),
                            .next_review = q.value("next_review").toDateTime(),
                            .created_at = q.value("created_at").toDateTime(),
                            .updated_at = q.value("updated_at").toDateTime(),
                            .front = q.value("front").toString(),
                            .back = q.value("back").toString()};

            result.push_back(std::move(card));
        }

        return result;
    });
}


core::IDeckStorage::Result<QVector<core::Card>> SqlDeckStorage::remove_cards(const QVector<core::Card::id_type>& ids) {
    return m_context.exec([this, &ids]() -> Result<QVector<core::Card>> {
        if (ids.isEmpty()) {
            return {};
        }

        QStringList placeholders;
        placeholders.reserve(ids.size());

        for (qsizetype i = 0; i < ids.size(); ++i) {
            placeholders.push_back(QString(":id_%1").arg(i));
        }

        const QString query_string = QString(R"(
            DELETE FROM cards
            WHERE id IN (%1)
            RETURNING
                id,
                deck_id,
                front,
                back,
                state,
                difficulty,
                interval,
                next_review,
                incorrect_streak,
                created_at,
                updated_at
        )")
                                         .arg(placeholders.join(", "));

        QSqlQuery q(m_db.raw_db());

        if (!q.prepare(query_string)) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::remove_cards(): failed to prepare query");
            return std::unexpected(error);
        }

        for (qsizetype i = 0; i < ids.size(); ++i) {
            q.bindValue(QString(":id_%1").arg(i), ids[i]);
        }

        if (!q.exec()) {
            auto error = from_sql_error(q.lastError(), "SqlDeckStorage::remove_cards(): failed to remove cards");
            return std::unexpected(error);
        }

        QVector<core::Card> removed_cards;
        removed_cards.reserve(ids.size());

        while (q.next()) {
            core::Card removed_card{.id = q.value("id").toInt(),
                                    .deck_id = q.value("deck_id").toInt(),
                                    .difficulty = q.value("difficulty").toFloat(),
                                    .state = static_cast<core::Card::State>(q.value("state").toInt()),
                                    .incorrect_streak = q.value("incorrect_streak").toInt(),
                                    .interval = q.value("interval").toInt(),
                                    .next_review = q.value("next_review").toDateTime(),
                                    .created_at = q.value("created_at").toDateTime(),
                                    .updated_at = q.value("updated_at").toDateTime(),
                                    .front = q.value("front").toString(),
                                    .back = q.value("back").toString()};

            removed_cards.push_back(std::move(removed_card));
        }

        return removed_cards;
    });
}


core::IDeckStorage::Error SqlDeckStorage::from_sql_error(const QSqlError& error, const QString& message) {
    bool      ok;
    const int code = error.nativeErrorCode().toInt(&ok);

    if (!ok) {
        return Error{.kind = Error::Kind::Unknown, .message = QString("%1: %2").arg(message, error.text())};
    }

    switch (code) {
    case SQLITE_CONSTRAINT_UNIQUE:
    case SQLITE_CONSTRAINT_PRIMARYKEY:
        return Error{.kind = Error::Kind::AlreadyExists, .message = QString("%1: %2").arg(message, error.text())};

    case SQLITE_CONSTRAINT_FOREIGNKEY:
    case SQLITE_CONSTRAINT_NOTNULL:
    case SQLITE_CONSTRAINT_CHECK:
    case SQLITE_MISMATCH:
    case SQLITE_RANGE:
        return Error{.kind = Error::Kind::InvalidArgument, .message = QString("%1: %2").arg(message, error.text())};

    case SQLITE_PERM:
    case SQLITE_READONLY:
        return Error{.kind = Error::Kind::PermissionDenied, .message = QString("%1: %2").arg(message, error.text())};

    case SQLITE_BUSY:
    case SQLITE_BUSY_TIMEOUT:
    case SQLITE_LOCKED:
    case SQLITE_IOERR:
    case SQLITE_FULL:
    case SQLITE_CANTOPEN:
        return Error{.kind = Error::Kind::Unavailable, .message = QString("%1: %2").arg(message, error.text())};

    default:
        return Error{.kind = Error::Kind::Unknown, .message = QString("%1: %2").arg(message, error.text())};
    }
}

} // namespace io
