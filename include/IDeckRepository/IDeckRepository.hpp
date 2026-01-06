// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <Entities/Card.hpp>        // for Card
#include <Entities/Deck.hpp>        // for Deck
#include <Entities/DeckSummary.hpp> // for DeckSummary
#include <QDateTime>                // for QDateTime
#include <QString>                  // for QString
#include <QVector>                  // for QVector
#include <expected>                 // for std::expected

namespace revise {

struct CardBackUpdate {
    int       cardId;
    QString   back;
    QDateTime updated_at;
};

/**
 * @brief Repository interface for deck/card persistence.
 *
 * Implementations (e.g. SqlDeckRepository) provide concrete storage logic.
 */
class IDeckRepository {
  public:
    virtual ~IDeckRepository() = default;

    virtual std::expected<QVector<DeckSummary>, QString> get_deck_summaries(const QDateTime& now) = 0;
    virtual std::expected<QVector<Card>, QString>        get_cards(int deckId) = 0;
    virtual std::expected<Deck, QString>                 get_deck(int deckId) = 0;

    virtual std::expected<int, QString>  create_deck(const Deck& deck) = 0;
    virtual std::expected<void, QString> update_deck(const Deck& deck) = 0;
    virtual std::expected<void, QString> delete_deck(int deckId) = 0;
    virtual std::expected<bool, QString> is_deck_exists(const QString& deck_name) = 0;
    virtual std::expected<int, QString> deck_id(const QString& deck_name) = 0;

    virtual std::expected<void, QString> update_card(const Card& card) = 0;
    virtual std::expected<void, QString> insert_cards_batch(int deckId, const QVector<Card>& cards) = 0;
    virtual std::expected<void, QString> update_card_backs_batch(const QVector<CardBackUpdate>& cards) = 0;
    virtual std::expected<void, QString> insert_card(int deckId, const Card& card) = 0;
    virtual std::expected<bool, QString> is_card_exists(int deckId, const QString& card_front) = 0;
    virtual std::expected<Card, QString> get_card(int cardId) = 0;
    virtual std::expected<void, QString> remove_card(int cardId) = 0;
};

} // namespace revise
