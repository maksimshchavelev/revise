// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QDateTime> // for QDateTime
#include <QString>   // for QString

namespace core {

/// @brief This struct describes a card
struct Card {
    using id_type = int;
    using deck_id_type = int;
    using difficulty_type = float;

    enum class State {
        New,      ///< New card
        Review,   ///< Need to review
        Incorrent ///< Incorrect card
    };

    id_type         id{0};               ///< ID of card
    id_type         global_id{0};        ///< Global ID
    deck_id_type    deck_id{0};          ///< ID of the deck to which the card belongs
    difficulty_type difficulty{2.5f};    ///< Card difficulty from `0` to `5`
    State           state{State::New};   ///< State of the card
    int             incorrect_streak{0}; ///< Number of consecutive times a user answered a card incorrectly
    int             interval{0};         ///< Interval
    int             time_limit{0};       ///< Time limit for response
    QDateTime       next_review;         ///< Date and time of next card viewing
    QDateTime       created_at;          ///< Date of creation of the card
    QDateTime       updated_at;          ///< Card update date
    QString         front;               ///< Front of the card
    QString         back;                ///< Back of the card

    bool operator==(const Card& other) const noexcept = default;
};

} // namespace core