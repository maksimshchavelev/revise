// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

namespace core {

/**
 * @brief Describes a deck
 */
struct Deck {
    QString name;                 ///< Deck name
    QString description;          ///< Description of the deck
    int     time_limit{0};        ///< Time limit for response
    int     new_limit{0};         ///< Limit on the number of new cards during training
    int     consolidate_limit{0}; ///< Limit on the number of reviewed cards during training
    int     incorrect_limit{0};   ///< Limit on the number of incorrect cards during training
    int     id{0};                ///< Deck ID
    int     global_id{0};         ///< Global ID
};

/**
 * @brief Describes a deck summary
 */
struct DeckSummary {
    Deck deck;                 ///< Information about the deck. See `core::Deck`.
    int  new_cards{0};         ///< Number of new cards included in today's selection
    int  consolidate_cards{0}; ///< Number of reviewed cards included in today's selection
    int  incorrect_cards{0};   ///< Number of incorrect cards included in today's sample
};

} // namespace core
