// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>    // for QAbstractListModel
#include <atomic>                // for std::atomic
#include <core/IDeckService.hpp> // for core::IDeckService

namespace ui {

class CardsModel final : public QAbstractListModel {
    Q_OBJECT

  public:
    Q_PROPERTY(int cardsCount READ cards_count NOTIFY updated FINAL);
    Q_PROPERTY(QString searchFront READ search_front WRITE set_search_front NOTIFY searchFrontChanged)

    CardsModel(core::IDeckService& service, QObject* parent = nullptr);

    enum CardRoles { FrontRole = Qt::UserRole + 1, BackRole, IdRole, NextReviewRole, DifficultyRole, StatusRole };

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
    void searchFrontChanged();
    void loadingStarted();
    void loadingFinished();

  private:
    core::IDeckService& m_deck_service;
    QVector<core::Card> m_cards;
    QString             m_search_front;
    std::atomic_int     m_last_deck_id{0};
    std::atomic_int     m_last_request_id{0};

    /**
     * @brief Get cards count
     * @return Count of the cards
     * @note First, call the `load` method to load information about the deck.
     */
    int cards_count() const noexcept;

    /// @brief Cards are searched by their front side. Returns the search query
    QString search_front() const;

    /// @brief Cards are searched by their front side. Sets the search query
    void set_search_front(QString front);
};

} // namespace ui
