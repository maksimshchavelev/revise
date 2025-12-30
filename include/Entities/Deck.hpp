// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

namespace revise {

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
};

} // namespace revise
