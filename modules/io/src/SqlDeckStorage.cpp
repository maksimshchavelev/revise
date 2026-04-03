// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/SqlDeckStorage.hpp" // for SqlDeckStorage
#include <QSqlError>             // for QSqlError
#include <QSqlQuery>             // for QSqlQuery

namespace io {

SqlDeckStorage::SqlDeckStorage(Database& db, DatabaseExecutionContext& context) : m_db(db), m_context(context) {}


std::expected<void, QString> SqlDeckStorage::create_decks(const QVector<core::Deck>& decks) {
    return m_context.exec([this, &decks]() -> std::expected<void, QString> {
        const bool need_transaction = decks.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin decks insertion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            INSERT INTO decks (name, description, time_limit, new_limit, consolidate_limit, incorrect_limit)
            VALUES (:name, :description, :time_limit, :new_limit, :consolidate_limit, :incorrect_limit)
        )");

        for (const auto& deck : decks) {
            q.bindValue(":name", deck.name);
            q.bindValue(":description", deck.description);
            q.bindValue(":time_limit", deck.time_limit);
            q.bindValue(":new_limit", deck.new_limit);
            q.bindValue(":consolidate_limit", deck.consolidate_limit);
            q.bindValue(":incorrect_limit", deck.incorrect_limit);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }

        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit decks insertion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}


std::expected<void, QString> SqlDeckStorage::update_decks(const QVector<core::Deck>& decks) {
    return m_context.exec([this, &decks]() -> std::expected<void, QString> {
        const bool need_transaction = decks.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin decks updating transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            UPDATE decks SET
                name = :name,
                description = :description,
                time_limit = :time_limit,
                new_limit = :new_limit,
                consolidate_limit = :consolidate_limit,
                incorrect_limit = :incorrect_limit
            WHERE id = :id
        )");

        for (const auto& deck : decks) {
            q.bindValue(":name", deck.name);
            q.bindValue(":description", deck.description);
            q.bindValue(":time_limit", deck.time_limit);
            q.bindValue(":new_limit", deck.new_limit);
            q.bindValue(":consolidate_limit", deck.consolidate_limit);
            q.bindValue(":incorrect_limit", deck.incorrect_limit);
            q.bindValue(":id", deck.id);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }


        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit decks updating transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}


std::expected<void, QString> SqlDeckStorage::delete_decks(const QVector<int>& ids) {
    return m_context.exec([this, &ids]() -> std::expected<void, QString> {
        const bool need_transaction = ids.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin decks deletion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare("DELETE FROM decks WHERE id = :id");

        for (const int id : ids) {
            q.bindValue(":id", id);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }

        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit decks deletion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}


std::expected<QVector<core::Deck>, QString> SqlDeckStorage::fetch_decks(const QVector<int>& ids) {
    return m_context.exec([this, &ids]() -> std::expected<QVector<core::Deck>, QString> {
        QVector<core::Deck> result;
        QSqlQuery           q(m_db.raw_db());

        q.prepare("SELECT * FROM decks WHERE id = :id");

        for (const int id : ids) {
            q.bindValue(":id", id);

            if (!q.exec()) {
                return std::unexpected(QString("Failed to fetch decks by id's, cause: %1").arg(q.lastError().text()));
            }

            while (q.next()) {
                core::Deck deck{.name = q.value("name").toString(),
                                .description = q.value("description").toString(),
                                .time_limit = q.value("time_limit").toInt(),
                                .new_limit = q.value("new_limit").toInt(),
                                .consolidate_limit = q.value("consolidate_limit").toInt(),
                                .incorrect_limit = q.value("incorrect_limit").toInt(),
                                .id = q.value("id").toInt(),
                                .global_id = q.value("global_id").toInt()};

                result.push_back(std::move(deck));
            }
        }

        return result;
    });
}


std::expected<QVector<core::Deck>, QString> SqlDeckStorage::fetch_decks(const QVector<QString>& names) {
    return m_context.exec([this, &names]() -> std::expected<QVector<core::Deck>, QString> {
        QVector<core::Deck> result;
        QSqlQuery           q(m_db.raw_db());

        q.prepare("SELECT * FROM decks WHERE name = :name");

        for (const auto& name : names) {
            q.bindValue(":name", name);

            if (!q.exec()) {
                return std::unexpected(QString("Failed to fetch decks by names, cause: %1").arg(q.lastError().text()));
            }

            while (q.next()) {
                core::Deck deck{.name = q.value("name").toString(),
                                .description = q.value("description").toString(),
                                .time_limit = q.value("time_limit").toInt(),
                                .new_limit = q.value("new_limit").toInt(),
                                .consolidate_limit = q.value("consolidate_limit").toInt(),
                                .incorrect_limit = q.value("incorrect_limit").toInt(),
                                .id = q.value("id").toInt(),
                                .global_id = q.value("global_id").toInt()};

                result.push_back(std::move(deck));
            }
        }

        return result;
    });
}


std::expected<QVector<core::Deck>, QString> SqlDeckStorage::fetch_decks() {
    return m_context.exec([this]() -> std::expected<QVector<core::Deck>, QString> {
        QVector<core::Deck> result;
        QSqlQuery           q(m_db.raw_db());

        result.reserve(256);

        q.prepare("SELECT * FROM decks");

        if (!q.exec()) {
            return std::unexpected(QString("Failed to fetch decks by names, cause: %1").arg(q.lastError().text()));
        }

        while (q.next()) {
            core::Deck deck{.name = q.value("name").toString(),
                            .description = q.value("description").toString(),
                            .time_limit = q.value("time_limit").toInt(),
                            .new_limit = q.value("new_limit").toInt(),
                            .consolidate_limit = q.value("consolidate_limit").toInt(),
                            .incorrect_limit = q.value("incorrect_limit").toInt(),
                            .id = q.value("id").toInt(),
                            .global_id = q.value("global_id").toInt()};

            result.push_back(std::move(deck));
        }

        return result;
    });
}


std::expected<void, QString> SqlDeckStorage::update_cards(const QVector<core::Card>& cards) {
    return m_context.exec([this, &cards]() -> std::expected<void, QString> {
        const bool need_transaction = cards.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin cards updating transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            UPDATE cards SET
                front = :front,
                back = :back,
                state = :state,
                incorrect_streak = :incorrect_streak,
                interval = :interval,
                difficulty = :difficulty,
                next_review = :next_review,
                updated_at = :updated_at
            WHERE id = :id
        )");

        for (const core::Card& card : cards) {
            q.bindValue(":front", card.front);
            q.bindValue(":back", card.back);
            q.bindValue(":state", card.state);
            q.bindValue(":incorrect_streak", card.incorrect_streak);
            q.bindValue(":interval", card.interval);
            q.bindValue(":difficulty", card.difficulty);
            q.bindValue(":next_review", card.next_review);
            q.bindValue(":updated_at", card.updated_at);
            q.bindValue(":id", card.id);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }

        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit cards updating transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}


std::expected<void, QString> SqlDeckStorage::insert_cards(const QVector<core::Card>& cards) {
    return m_context.exec([this, &cards]() -> std::expected<void, QString> {
        const bool need_transaction = cards.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin cards insertion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare(R"(
            INSERT INTO cards (deck_id, front, back, state, difficulty, interval, next_review, incorrect_streak, created_at, updated_at)
            VALUES (:deck_id, :front, :back, :state, :difficulty, :interval, :next_review, :incorrect_streak, :created_at, :updated_at)
        )");

        for (const core::Card& card : cards) {
            q.bindValue(":deck_id", card.deck_id);
            q.bindValue(":front", card.front);
            q.bindValue(":back", card.back);
            q.bindValue(":state", card.state);
            q.bindValue(":difficulty", card.difficulty);
            q.bindValue(":interval", card.interval);
            q.bindValue(":next_review", card.next_review);
            q.bindValue(":incorrect_streak", card.incorrect_streak);
            q.bindValue(":created_at", card.created_at);
            q.bindValue(":updated_at", card.updated_at);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }

        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit cards insertion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}


std::expected<QVector<core::Card>, QString> SqlDeckStorage::fetch_cards(int deck_id) {
    return m_context.exec([this, deck_id]() -> std::expected<QVector<core::Card>, QString> {
        QVector<core::Card> result;
        QSqlQuery           q(m_db.raw_db());

        q.prepare("SELECT * FROM cards WHERE deck_id = :deck_id");
        q.bindValue(":deck_id", deck_id);

        if (!q.exec()) {
            return std::unexpected(QString("Failed to fetch cards by deck ids, cause: %1").arg(q.lastError().text()));
        }

        while (q.next()) {
            core::Card card{.id = q.value("id").toInt(),
                            .deck_id = q.value("deck_id").toInt(),
                            .state = q.value("state").toInt(),
                            .incorrect_streak = q.value("incorrect_streak").toInt(),
                            .interval = q.value("interval").toInt(),
                            .difficulty = q.value("difficulty").toFloat(),
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


std::expected<QVector<core::Card>, QString> SqlDeckStorage::fetch_cards(const QVector<int>& ids) {
    return m_context.exec([this, &ids]() -> std::expected<QVector<core::Card>, QString> {
        QVector<core::Card> result;
        QSqlQuery           q(m_db.raw_db());

        q.prepare("SELECT * FROM cards WHERE id = :id");

        for (const int id : ids) {
            q.bindValue(":id", id);

            if (!q.exec()) {
                return std::unexpected(QString("Failed to fetch cards by ids, cause: %1").arg(q.lastError().text()));
            }

            while (q.next()) {
                core::Card card{.id = q.value("id").toInt(),
                                .deck_id = q.value("deck_id").toInt(),
                                .state = q.value("state").toInt(),
                                .incorrect_streak = q.value("incorrect_streak").toInt(),
                                .interval = q.value("interval").toInt(),
                                .difficulty = q.value("difficulty").toFloat(),
                                .next_review = q.value("next_review").toDateTime(),
                                .created_at = q.value("created_at").toDateTime(),
                                .updated_at = q.value("updated_at").toDateTime(),
                                .front = q.value("front").toString(),
                                .back = q.value("back").toString()};

                result.push_back(std::move(card));
            }
        }

        return result;
    });
}


std::expected<void, QString> SqlDeckStorage::remove_cards(const QVector<int>& ids) {
    return m_context.exec([this, &ids]() -> std::expected<void, QString> {
        const bool need_transaction = ids.size() > 1;

        if (need_transaction) {
            if (!m_db.begin_transaction()) {
                return std::unexpected(
                    QString("Failed to begin cards deletion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        QSqlQuery q(m_db.raw_db());

        q.prepare("DELETE FROM cards WHERE id = :id");

        for (const int id : ids) {
            q.bindValue(":id", id);

            if (!q.exec()) {
                if (need_transaction) {
                    m_db.rollback_transaction();
                }
                return std::unexpected(q.lastError().text());
            }
        }

        if (need_transaction) {
            if (!m_db.commit_transaction()) {
                return std::unexpected(
                    QString("Failed to commit cards deletion transaction, cause: %1").arg(m_db.last_error_text()));
            }
        }

        return {};
    });
}

} // namespace io