// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>    // for QAbstractListModel
#include <core/IDeckService.hpp> // for core::IDeckService

namespace ui {

class CardsModel final : public QAbstractListModel {
    Q_OBJECT

  public:
    Q_PROPERTY(int cardsCount READ cards_count NOTIFY updated FINAL);

    CardsModel(core::IDeckService& service, QObject* parent = nullptr);

    enum CardRoles { FrontRole = Qt::UserRole + 1, BackRole, IdRole, NextReviewRole, DifficultyRole };

    /**
     * @brief Set current deck. The cards will be loaded specifically for this deck.
     * @param deckId ID of deck
     */
    Q_INVOKABLE void setDeck(int deckId);

    int                    rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    void updated();

  private:
    core::IDeckService& m_deck_service;
    QVector<core::Card> m_cards;
    int                 m_last_deck_id{0};

    /**
     * @brief Get cards count
     * @return Count of the cards
     * @note First, call the `load` method to load information about the deck.
     */
    int cards_count() const noexcept;
};

} // namespace ui
