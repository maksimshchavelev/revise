// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Card.hpp" // for Card
#include "Deck.hpp" // for Deck
#include <QString>  // for QString
#include <QVector>  // for QVector
#include <expected> // for std::expected

namespace core {

/**
 * @brief .
 *
 * Implementations provide concrete storage logic.
 */
class IDeckStorage {
  public:
    virtual ~IDeckStorage() = default;

    /**
     * @brief Create new decks from a vector
     * @param decks Vector of deck objects (`core::Deck`)
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> create_decks(const QVector<Deck>& decks) = 0;

    /**
     * @brief Update existing decks from a vector
     * @param decks Vector of deck objects (`core::Deck`)
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> update_decks(const QVector<Deck>& decks) = 0;

    /**
     * @brief Delete decks with the specified IDs
     * @param ids Vector of deck IDs to be deleted
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> delete_decks(const QVector<int>& ids) = 0;

    /**
     * @brief Fetch decks by specific IDs
     * @param ids Vector of deck IDs to fetch
     * @return `std::expected<QVector<Deck>, QString>` containing fetched decks on success,
     *         or an error description on failure
     */
    virtual std::expected<QVector<Deck>, QString> fetch_decks(const QVector<int>& ids) = 0;

    /**
     * @brief Fetch decks by specific names
     * @param names Vector of deck names to fetch
     * @return `std::expected<QVector<Deck>, QString>` containing fetched decks on success,
     *         or an error description on failure
     */
    virtual std::expected<QVector<Deck>, QString> fetch_decks(const QVector<QString>& names) = 0;

    /**
     * @brief Get *all* decks
     * @return `std::expected<QVector<Deck>, QString>` containing fetched decks on success,
     *         or an error description on failure
     */
    virtual std::expected<QVector<Deck>, QString> fetch_decks() = 0;

    /**
     * @brief Update cards from a vector
     * @param cards Vector of card objects (`core::Card`) to be updated
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> update_cards(const QVector<Card>& cards) = 0;

    /**
     * @brief Insert new cards from a vector
     * @param cards Vector of card objects (`core::Card`) to be inserted
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> insert_cards(const QVector<Card>& cards) = 0;

    /**
     * @brief Fetch all cards from a specific deck
     * @param deck_id ID of the deck
     * @return `std::expected<QVector<Card>, QString>` containing cards on success,
     *         or an error description on failure
     */
    virtual std::expected<QVector<Card>, QString> fetch_cards(int deck_id) = 0;

    /**
     * @brief Fetch cards by specific IDs
     * @param ids Vector of card IDs to fetch
     * @return `std::expected<QVector<Card>, QString>` containing cards on success,
     *         or an error description on failure
     */
    virtual std::expected<QVector<Card>, QString> fetch_cards(const QVector<int>& ids) = 0;

    /**
     * @brief Remove cards with the specified IDs
     * @param ids Vector of card IDs to remove
     * @return `std::expected<void, QString>` empty on success, or with error description on failure
     */
    virtual std::expected<void, QString> remove_cards(const QVector<int>& ids) = 0;
};

} // namespace core
