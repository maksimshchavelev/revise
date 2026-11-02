// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>                 // for QAbstractListModel
#include <QSet>                               // for QSet
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <atomic>                             // for std::atomic
#include <core/service/IDeckService.hpp>      // for core::IDeckService
#include <wrappers/CardWrapper.hpp>           // for ui::Card

namespace ui {

class CardsModel;

/// @brief List model of cards that match a front-side search query.
/// @details Performs the initial search through the deck service using
///          `search_cards` with a combination of deck-id and front-search
///          filters. Subsequent card_created, card_removed and card_updated
///          events are applied incrementally, so the scroll position of the
///          view is preserved. Selection state is mirrored from CardsModel
///          through the cardSelectionChanged signal and cleared when the
///          deck changes.
class CardSearchModel final : public QAbstractListModel {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("CardSearchModel is provided by C++")

  public:
    // How many cards were found during the search?
    Q_PROPERTY(int foundCardsCount READ found_cards_count NOTIFY updated FINAL);

    // Search query based on the front of the card
    Q_PROPERTY(QString searchFront READ search_front WRITE set_search_front NOTIFY searchFrontChanged)

    /// @brief Construct the search model
    /// @param service Deck service used to perform the search
    /// @param cards_model Main cards model, used for deck tracking and selection sync
    /// @param parent Parent object
    explicit CardSearchModel(core::IDeckService& service, CardsModel& cards_model, QObject* parent = nullptr);

    /// @brief Roles of cards search model
    enum CardRoles {
        FrontRole = Qt::UserRole + 1, ///< Front of a card
        BackRole,                     ///< Back of a card
        IdRole,                       ///< Local ID of a card
        NextReviewRole,               ///< Date of next review
        DifficultyRole,               ///< Difficulty of a card
        StatusRole,                   ///< Status of a card
        SelectedRole                  ///< Is a card selected via UI?
    };

    /// @brief Get the current search query
    /// @return The search query
    [[nodiscard]] QString search_front() const;

    /// @brief Set the search query and re-run the search
    /// @param front Search query
    void set_search_front(QString front);

    /// @brief Get the number of cards that match the current query
    /// @return Count of matching cards
    [[nodiscard]] int found_cards_count() const noexcept;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    /// @brief Emitted when the search result or its count changes
    void updated();

    /// @brief Emitted when the search query changes
    void searchFrontChanged();

    /// @brief Emitted before the model starts searching in the background
    void loadingStarted();

    /// @brief Emitted after the search has finished
    void loadingFinished();

  private:
    /// @brief Re-run the current search, or clear the result if the query
    ///        is empty. Any in-flight search is invalidated.
    void reload();

    /// @brief Handle card_created event: insert a row at the end if the
    ///        new card matches the current query
    void on_card_created(const core::Card& card);

    /// @brief Handle card_removed event: remove the matching row
    void on_card_removed(const core::Card& card);

    /// @brief Handle card_updated event: update, insert, or remove the row
    ///        depending on whether the card matched before and matches now
    void on_card_updated(const core::Card& card);

    /// @brief Mirror a selection change from CardsModel into this model
    /// @param cardId The local ID of the card
    /// @param selected New selection state
    void on_card_selection_changed(int cardId, bool selected);

    /// @brief Check whether a card matches the current search query
    /// @param card Card to check
    /// @return `true` if the card belongs to the current deck and its
    ///         front contains the current query
    [[nodiscard]] bool matches(const core::Card& card) const;

    core::IDeckService& m_deck_service;   ///< Deck service
    CardsModel& m_cards_model;            ///< Main cards model (deck state source)
    QVector<Card> m_cards;                ///< Found cards
    QSet<int> m_selected_ids;             ///< Selected card IDs, mirrored from CardsModel
    QString m_search_front;               ///< Search query
    std::atomic_int m_last_request_id{0}; ///< Last search request ID
};

} // namespace ui
