// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "core/Card.hpp" // for Card
#include "core/Deck.hpp" // for Deck
#include <QString>       // for QString
#include <QVector>       // for QVector
#include <expected>      // for std::expected

namespace core {

/// @brief Deck and Card Storage
class IDeckStorage {
  public:
    /// @brief Describes a deck storage error
    struct Error {
        enum class Kind {
            Unavailable,      ///< Storage backend is unavailable
            NotFound,         ///< Requested entity was not found
            AlreadyExists,    ///< Entity already exists
            InvalidArgument,  ///< Input data is invalid
            PermissionDenied, ///< Operation is not allowed
            Unknown           ///< Unclassified error
        };

        Kind    kind;
        QString message;
    };

    using ErrorKind = Error::Kind;
    template <typename T> using Result = std::expected<T, Error>;

    virtual ~IDeckStorage() = default;

    /**
     * @brief Create new decks from a vector
     * @param decks Vector of deck objects (`core::Deck`)
     * @return Created decks
     */
    virtual Result<QVector<Deck>> create_decks(const QVector<Deck>& decks) = 0;

    /**
     * @brief Update existing decks from a vector
     * @param decks Vector of deck objects (`core::Deck`)
     * @return Updated decks
     */
    virtual Result<QVector<Deck>> update_decks(const QVector<Deck>& decks) = 0;

    /**
     * @brief Remove decks with the specified IDs
     * @param ids Vector of deck IDs to be deleted
     * @return Deleted decks
     */
    virtual Result<QVector<Deck>> remove_decks(const QVector<Deck::id_type>& ids) = 0;

    /**
     * @brief Fetch decks by specific IDs
     * @param ids Vector of deck IDs to fetch
     * @return QVector of fetched decks
     */
    virtual Result<QVector<Deck>> fetch_decks(const QVector<Deck::id_type>& ids) const = 0;

    /**
     * @brief Fetch all decks
     * @return QVector of decks
     */
    virtual Result<QVector<Deck>> fetch_decks() const = 0;

    /**
     * @brief Update cards from a vector
     * @param cards Vector of card objects (`core::Card`) to be updated
     * @return Updated cards
     */
    virtual Result<QVector<Card>> update_cards(const QVector<Card>& cards) = 0;

    /**
     * @brief Create new cards from a vector
     * @param cards Vector of card objects (`core::Card`) to be created
     * @return Created cards
     */
    virtual Result<QVector<Card>> create_cards(const QVector<Card>& cards) = 0;

    /**
     * @brief Fetch all cards from a specific deck
     * @param deck_id ID of the deck
     * @return QVector of cards
     */
    virtual Result<QVector<Card>> fetch_cards(Deck::id_type deck_id) const = 0;

    /**
     * @brief Fetch cards by specific IDs
     * @param ids Vector of card IDs to fetch
     * @return QVector of cards
     */
    virtual Result<QVector<Card>> fetch_cards(const QVector<Card::id_type>& ids) const = 0;

    /**
     * @brief Fetch all cards
     * @return QVector of cards
     */
    virtual Result<QVector<Card>> fetch_cards() const = 0;

    /**
     * @brief Remove cards with the specified IDs
     * @param ids Vector of card IDs to remove
     * @return Removed cards
     */
    virtual Result<QVector<Card>> remove_cards(const QVector<Card::id_type>& ids) = 0;
};

} // namespace core
