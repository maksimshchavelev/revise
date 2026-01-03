// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>   // for QString
#include <QMetaType> // for Q_GADGET

/**
 * @brief Describes a deck
 */
struct Deck {
    Q_GADGET

public:
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(int timeLimit MEMBER time_limit)
    Q_PROPERTY(int newLimit MEMBER new_limit)
    Q_PROPERTY(int consolidateLimit MEMBER consolidate_limit)
    Q_PROPERTY(int incorrectLimit MEMBER incorrect_limit)
    Q_PROPERTY(int id MEMBER id)

    QString name;                 ///< Deck name
    QString description;          ///< Description of the deck
    int     time_limit{0};        ///< Time limit for response
    int     new_limit{0};         ///< Limit on the number of new cards during training
    int     consolidate_limit{0}; ///< Limit on the number of reviewed cards during training
    int     incorrect_limit{0};   ///< Limit on the number of incorrect cards during training
    int     id{0};                ///< Deck ID
};

Q_DECLARE_METATYPE(Deck)

