// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A model that stores a list of cards. Can be used in QML.

#pragma once

#include <Database/Database.hpp>       // for Database
#include <DeckService/DeckService.hpp> // for DeckService
#include <QAbstractListModel>          // for QAbstractListModel

namespace revise {

class CardsModel : public QAbstractListModel {
    Q_OBJECT

public:
    Q_PROPERTY(int cardsCount READ cards_count NOTIFY dataChanged);

    CardsModel(DeckService& service);

    enum CardRoles {
        FrontRole = Qt::UserRole + 1,
        BackRole,
        IdRole
    };

    /**
     * @brief Load cards for deck
     * @param deckId ID of deck
     */
    Q_INVOKABLE void load(int deckId);

    /**
     * @brief Get cards count
     * @return Count of the cards
     * @note First, call the `load` method to load information about the deck.
     */
    Q_INVOKABLE int cards_count() const noexcept;

    int                    rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void dataChanged();

private:
    DeckService&  m_deck_service;
    QVector<Card> m_cards;
    int           m_last_deckId{0};
};

} // namespace revise
