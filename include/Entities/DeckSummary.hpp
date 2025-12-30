// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Deck.hpp" // for Deck

namespace revise {

/**
 * @brief Describes a deck summary
 */
struct DeckSummary {
    Deck deck;                 ///< Information about the deck. See `Deck`.
    int  new_cards{0};         ///< Number of new cards included in today's selection
    int  consolidate_cards{0}; ///< Number of reviewed cards included in today's selection
    int  incorrect_cards{0};   ///< Number of incorrect cards included in today's sample
};

} // namespace revise
