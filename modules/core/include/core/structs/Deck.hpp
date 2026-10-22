// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

namespace core {

/// @brief This struct describes a deck
struct Deck {
    using id_type = int;

    QString name;               ///< Deck name
    QString description;        ///< Description of the deck
    id_type id{0};              ///< Deck ID
    id_type global_id{0};       ///< Global ID
    int     time_limit{0};      ///< Time limit for response
    int     new_limit{0};       ///< Limit on the number of new cards during training
    int     review_limit{0};    ///< Limit on the number of reviewed cards during training
    int     incorrect_limit{0}; ///< Limit on the number of incorrect cards during training

    bool operator==(const Deck& other) const noexcept = default;
};

/// @brief This struct describes a deck summary
struct DeckSummary {
    Deck deck;               ///< Information about the deck. See `core::Deck`.
    int  new_cards{0};       ///< Number of new cards included in today's selection
    int  review_cards{0};    ///< Number of reviewed cards included in today's selection
    int  incorrect_cards{0}; ///< Number of incorrect cards included in today's sample

    bool operator==(const DeckSummary& other) const noexcept = default;
};

} // namespace core
