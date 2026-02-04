// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QDateTime> // for QDateTime
#include <QString>   // for QString

namespace core {

/**
 * @brief Describes a card
 */
struct Card {
    int       id{0};               ///< ID of card
    int       global_id{0};        ///< Global ID
    int       deck_id{0};          ///< ID of the deck to which the card belongs
    int       state{0};            ///< State of the card. `0` - new card, `1` - need to review, `2` - need to relearn
    int       incorrect_streak{0}; ///< Number of consecutive times a user answered a card incorrectly
    int       interval{0};         ///< Interval
    int       time_limit{0};       ///< Time limit for response
    float     difficulty{0};       ///< Card difficulty from `0` to `5`
    QDateTime next_review;         ///< Date and time of next card viewing
    QDateTime created_at;          ///< Date of creation of the card
    QDateTime updated_at;          ///< Card update date
    QString   front;               ///< Front of the card
    QString   back;                ///< Back of the card
};

} // namespace core