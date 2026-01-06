// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QDateTime> // for QDateTime
#include <QString>   // for QString

/**
 * @brief Describes a card
 */
struct Card {
Q_GADGET

public:
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(int deckId MEMBER deck_id)
    Q_PROPERTY(int state MEMBER state)
    Q_PROPERTY(int incorrectStreak MEMBER incorrect_streak)
    Q_PROPERTY(int interval MEMBER interval)
    Q_PROPERTY(int timeLimit MEMBER time_limit)
    Q_PROPERTY(float difficulty MEMBER difficulty)
    Q_PROPERTY(QDateTime nextReview MEMBER next_review)
    Q_PROPERTY(QDateTime createdAt MEMBER created_at)
    Q_PROPERTY(QDateTime updatedAt MEMBER updated_at)
    Q_PROPERTY(QString front MEMBER front)
    Q_PROPERTY(QString back MEMBER back)

    int       id{0};               ///< ID of card
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

Q_DECLARE_METATYPE(Card)
