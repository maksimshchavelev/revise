// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <QSqlError>                               // for QSqlError
#include <QSqlQuery>                               // for QSqlQuery
#include <SqlDeckRepository/SqlDeckRepository.hpp> // for SqlDeckRepository header

namespace revise {

// Public method
SqlDeckRepository::SqlDeckRepository(Database& driver, QObject* parent) : QObject(parent), m_db(driver) {}


// Public method
std::expected<QVector<DeckSummary>, QString> SqlDeckRepository::get_deck_summaries(const QDateTime& now) {
    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        SELECT
            d.id AS deck_id,
            d.name AS deck_name,
            d.description AS deck_description,
            d.time_limit AS deck_time_limit,
            d.new_limit AS deck_new_limit,
            d.consolidate_limit AS deck_consolidate_limit,
            d.incorrect_limit AS deck_incorrect_limit,
            COALESCE( MIN( SUM(CASE WHEN c.state = 0 AND c.next_review <= :now THEN 1 ELSE 0 END), d.new_limit ), 0) AS new_cards,
            COALESCE( MIN( SUM(CASE WHEN c.state = 1 AND c.next_review <= :now THEN 1 ELSE 0 END), d.consolidate_limit ), 0) AS consolidate_cards,
            COALESCE( MIN( SUM(CASE WHEN c.state = 2 AND c.next_review <= :now THEN 1 ELSE 0 END), d.incorrect_limit ), 0) AS incorrect_cards
        FROM decks d
        LEFT JOIN cards c ON c.deck_id = d.id
        GROUP BY d.id
        ORDER BY (new_cards + consolidate_cards + incorrect_cards) DESC
    )");

    q.bindValue(":now", now);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    QVector<DeckSummary> result;

    while (q.next()) {
        Deck di;
        di.id = q.value("deck_id").toInt();
        di.name = q.value("deck_name").toString();
        di.description = q.value("deck_description").toString();
        di.time_limit = q.value("deck_time_limit").toInt();
        di.new_limit = q.value("deck_new_limit").toInt();
        di.consolidate_limit = q.value("deck_consolidate_limit").toInt();
        di.incorrect_limit = q.value("deck_incorrect_limit").toInt();

        DeckSummary s;
        s.deck = std::move(di);
        s.new_cards = q.value("new_cards").toInt();
        s.consolidate_cards = q.value("consolidate_cards").toInt();
        s.incorrect_cards = q.value("incorrect_cards").toInt();

        result.push_back(std::move(s));
    }

    return result;
}


// Public method
std::expected<QVector<Card>, QString> SqlDeckRepository::get_cards(int deckId) {
    QSqlQuery     q(m_db.raw_db());
    QVector<Card> result;

    q.prepare(R"(
        SELECT cards.*, decks.time_limit
        FROM cards
        JOIN decks ON cards.deck_id = decks.id
        WHERE cards.deck_id = :deck_id
    )");

    q.bindValue(":deck_id", deckId);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    while (q.next()) {
        result.append(Card{.id = q.value("id").toInt(),
                           .deck_id = q.value("deck_id").toInt(),
                           .state = q.value("state").toInt(),
                           .incorrect_streak = q.value("incorrect_streak").toInt(),
                           .interval = q.value("interval").toInt(),
                           .time_limit = q.value("time_limit").toInt(),
                           .difficulty = q.value("difficulty").toFloat(),
                           .next_review = q.value("next_review").toDateTime(),
                           .created_at = q.value("created_at").toDateTime(),
                           .updated_at = q.value("updated_at").toDateTime(),
                           .front = q.value("front").toString(),
                           .back = q.value("back").toString()});
    }

    return result;
}


// Public method
std::expected<Deck, QString> SqlDeckRepository::get_deck(int deckId) {
    QSqlQuery q(m_db.raw_db());

    q.prepare("SELECT * FROM decks WHERE id = :id");
    q.bindValue(":id", deckId);
    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected(QString("Deck not found: %1").arg(deckId));
    }

    return Deck{.name = q.value("name").toString(),
                .description = q.value("description").toString(),
                .time_limit = q.value("time_limit").toInt(),
                .new_limit = q.value("new_limit").toInt(),
                .consolidate_limit = q.value("consolidate_limit").toInt(),
                .incorrect_limit = q.value("incorrect_limit").toInt(),
                .id = q.value("id").toInt()};
}


// Public method
std::expected<int, QString> SqlDeckRepository::create_deck(const Deck &deck)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        INSERT INTO decks (name, description, time_limit, new_limit, consolidate_limit, incorrect_limit)
        VALUES (:name, :description, :time_limit, :new_limit, :consolidate_limit, :incorrect_limit)
    )");

    q.bindValue(":name", deck.name);
    q.bindValue(":description", deck.description);
    q.bindValue(":time_limit", deck.time_limit);
    q.bindValue(":new_limit", deck.new_limit);
    q.bindValue(":consolidate_limit", deck.consolidate_limit);
    q.bindValue(":incorrect_limit", deck.incorrect_limit);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    QVariant lastId = q.lastInsertId();
    emit data_changed();
    return lastId.toInt();
}


// Public method
std::expected<void, QString> SqlDeckRepository::update_deck(const Deck &deck)
{
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

    q.bindValue(":name", deck.name);
    q.bindValue(":description", deck.description);
    q.bindValue(":time_limit", deck.time_limit);
    q.bindValue(":new_limit", deck.new_limit);
    q.bindValue(":consolidate_limit", deck.consolidate_limit);
    q.bindValue(":incorrect_limit", deck.incorrect_limit);
    q.bindValue(":id", deck.id);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    emit data_changed();
    return {};
}


// Public method
std::expected<void, QString> SqlDeckRepository::delete_deck(int deckId)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare("DELETE FROM decks WHERE id = :id");
    q.bindValue(":id", deckId);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (auto res = m_media.remove_media(deckId); !res.has_value()) {
        return std::unexpected(res.error());
    }

    emit data_changed();
    return {};
}


// Public method
std::expected<bool, QString> SqlDeckRepository::is_deck_exists(const QString& deck_name)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare("SELECT COUNT(*) FROM decks WHERE name = :name");
    q.bindValue(":name", deck_name);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected("Failed to read query result");
    }

    int count = q.value(0).toInt();
    return count > 0;
}


// Public method
std::expected<int, QString> SqlDeckRepository::deck_id(const QString& deck_name)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare("SELECT id FROM decks WHERE name = :name");
    q.bindValue(":name", deck_name);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected("The query returned an empty result");
    }

    return q.value("id").toInt();
}


// Public method
std::expected<void, QString> SqlDeckRepository::update_card(const Card& card) {
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
        return std::unexpected(q.lastError().text());
    }

    emit data_changed();
    return {};
}


// Public method
std::expected<void, QString> SqlDeckRepository::insert_cards_batch(int deckId, const QVector<Card>& cards) {
    // do batch insert inside one transaction
    auto start = m_db.begin_transaction();

    if (!start.has_value()) {
        return std::unexpected(start.error());
    }

    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        INSERT INTO cards (deck_id, front, back, state, difficulty, interval, next_review, incorrect_streak, created_at, updated_at)
        VALUES (:deck_id, :front, :back, :state, :difficulty, :interval, :next_review, :incorrect_streak, :created_at, :updated_at)
    )");

    for (const auto& c : cards) {
        q.bindValue(":deck_id", deckId);
        q.bindValue(":front", c.front);
        q.bindValue(":back", c.back);
        q.bindValue(":state", c.state);
        q.bindValue(":difficulty", c.difficulty);
        q.bindValue(":interval", c.interval);
        q.bindValue(":next_review", c.next_review);
        q.bindValue(":incorrect_streak", c.incorrect_streak);
        q.bindValue(":created_at", c.created_at);
        q.bindValue(":updated_at", c.updated_at);

        if (!q.exec()) {
            m_db.rollback_transaction();
            return std::unexpected(q.lastError().text());
        }
    }

    auto commit = m_db.commit_transaction();
    if (!commit.has_value()) {
        m_db.rollback_transaction();
        return std::unexpected(commit.error());
    }

    emit data_changed();
    return {};
}

// Public method
std::expected<void, QString> SqlDeckRepository::update_card_backs_batch(const QVector<CardBackUpdate>& cards)
{
    // do batch insert inside one transaction
    auto start = m_db.begin_transaction();

    if (!start.has_value()) {
        return std::unexpected(start.error());
    }

    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        UPDATE cards
        SET back = :back, updated_at = :updated_at
        WHERE id = :id
    )");

    for (const auto& c : cards) {
        q.bindValue(":id", c.cardId);
        q.bindValue(":back", c.back);
        q.bindValue(":updated_at", c.updated_at);

        if (!q.exec()) {
            m_db.rollback_transaction();
            return std::unexpected(q.lastError().text());
        }
    }

    auto commit = m_db.commit_transaction();
    if (!commit.has_value()) {
        m_db.rollback_transaction();
        return std::unexpected(commit.error());
    }

    emit data_changed();
    return {};
}

// Public method
std::expected<void, QString> SqlDeckRepository::insert_card(int deckId, const Card &card)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        INSERT INTO cards (deck_id, front, back, state, difficulty, interval, next_review, incorrect_streak, created_at, updated_at)
        VALUES (:deck_id, :front, :back, :state, :difficulty, :interval, :next_review, :incorrect_streak, :created_at, :updated_at)
    )");

    q.bindValue(":deck_id", deckId);
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
        return std::unexpected(q.lastError().text());
    }

    emit data_changed();
    return {};
}

// Public method
std::expected<bool, QString> SqlDeckRepository::is_card_exists(int deckId, const QString& card_front)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare("SELECT COUNT(*) FROM cards WHERE front = :front AND deck_id = :deck_id");
    q.bindValue(":front", card_front);
    q.bindValue(":deck_id", deckId);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected("Failed to read query result");
    }

    int count = q.value(0).toInt();
    return count > 0;
}

// Public method
std::expected<Card, QString> SqlDeckRepository::get_card(int cardId)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        SELECT * FROM cards WHERE id = :cardId
    )");

    q.bindValue(":cardId", cardId);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    if (!q.next()) {
        return std::unexpected(QString("No card with id = %1 fetched").arg(cardId));
    }

    return Card{.id = q.value("id").toInt(),
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
}

// Public method
std::expected<void, QString> SqlDeckRepository::remove_card(int cardId)
{
    QSqlQuery q(m_db.raw_db());

    q.prepare(R"(
        DELETE FROM cards WHERE id = :cardId
    )");
    q.bindValue(":cardId", cardId);

    if (!q.exec()) {
        return std::unexpected(q.lastError().text());
    }

    return {};
}

} // namespace revise
