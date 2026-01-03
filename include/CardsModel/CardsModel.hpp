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

    int                    rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    DeckService&  m_deck_service;
    QVector<Card> m_cards;
};

} // namespace revise
