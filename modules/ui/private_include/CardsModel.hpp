// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>                 // for QAbstractListModel
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <atomic>                             // for std::atomic
#include <core/service/IDeckService.hpp>      // for core::IDeckService
#include <wrappers/CardWrapper.hpp>           // for ui::Card

namespace ui {

/// @brief Flat list model of cards for the currently selected deck.
/// @details Loads all cards of a deck from the deck service when the deck
///          is set, and keeps the list in sync with card_created, card_removed
///          and card_updated events using incremental row updates, so the
///          scroll position of the view is preserved. Selection state lives
///          in the Card wrapper itself and is exposed via SelectedRole.
class CardsModel final : public QAbstractListModel {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("CardsModel is provided by C++")

  public:
    // How many cards are there in the deck in total?
    Q_PROPERTY(int cardsCount READ cards_count NOTIFY updated FINAL);

    // Count of all new cards in the deck
    Q_PROPERTY(int newCardsCount READ new_cards_count NOTIFY updated FINAL);

    // Count of all review cards in the deck
    Q_PROPERTY(int reviewCardsCount READ review_cards_count NOTIFY updated FINAL);

    // Count of all incorrect cards in the deck
    Q_PROPERTY(int incorrectCardsCount READ incorrect_cards_count NOTIFY updated FINAL);

    // Count of selected cards
    Q_PROPERTY(int selectedCount READ selected_count NOTIFY cardSelectionChanged FINAL)

    /// @brief Construct the model and subscribe to deck service events
    /// @param service Deck service used to fetch cards
    /// @param parent Parent object
    explicit CardsModel(core::IDeckService& service, QObject* parent = nullptr);

    /// @brief Roles of cards model
    enum CardRoles {
        FrontRole = Qt::UserRole + 1, ///< Front of a card
        BackRole,                     ///< Back of a card
        IdRole,                       ///< Local ID of a card
        NextReviewRole,               ///< Date of next review
        DifficultyRole,               ///< Difficulty of a card
        StatusRole,                   ///< Status of a card
        SelectedRole                  ///< Is a card selected via UI?
    };

    /// @brief Set current deck. The cards will be loaded for this deck.
    /// @param deckId ID of deck
    Q_INVOKABLE void setDeck(int deckId);

    /// @brief Select a card from the list, for example, to perform batch actions
    /// @param cardId The local ID of the card you need to select
    Q_INVOKABLE void selectCard(int cardId) noexcept;

    /// @brief Deselect the card
    /// @param cardId The local ID of the card you need to deselect
    Q_INVOKABLE void deselectCard(int cardId) noexcept;

    /// @brief Select all cards, for example, to perform batch actions
    Q_INVOKABLE void selectAll();

    /// @brief Deselect all cards
    Q_INVOKABLE void deselectAll();

    /// @brief Get ID's of selected cards
    Q_INVOKABLE QVector<int> selectedIds();

    /// @brief Get ID's of all cards in the deck
    Q_INVOKABLE QVector<int> ids();

    /// @brief Get the ID of the currently loaded deck
    /// @return Deck ID
    [[nodiscard]] int deck_id() const noexcept;

    [[nodiscard]] int                    rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant               data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  signals:
    /// @brief Emitted when the card list or the cards count changes
    void updated();

    /// @brief Emitted when the current deck changes
    /// @param deckId New deck ID
    void deckChanged(int deckId);

    /// @brief Emitted when a card's selection state changes
    /// @param cardId The local ID of the card
    /// @param selected New selection state
    void cardSelectionChanged(int cardId, bool selected);

    /// @brief Emitted before the model starts loading cards in the background
    void loadingStarted();

    /// @brief Emitted after the model has finished loading cards
    void loadingFinished();

  private:
    /// @brief Handle card_created event
    void on_card_created(const core::Card& card);

    /// @brief Handle card_removed event
    void on_card_removed(const core::Card& card);

    /// @brief Handle card_updated or cards_updated events
    void on_cards_updated(const QVector<core::Card>& cards);

    core::IDeckService& m_deck_service;       ///< Deck service
    QVector<Card>       m_cards;              ///< All cards
    int                 m_selected_count{0};  ///< Count of selected cards
    std::atomic_int     m_last_deck_id{0};    ///< ID of the most recently loaded deck
    std::atomic_int     m_last_request_id{0}; ///< Last deck load request ID

    /// @brief Get cards count
    /// @return Count of the cards
    /// @note First, call the `setDeck` method to load the cards.
    [[nodiscard]] int cards_count() const noexcept;

    /// @brief Get <b>new</b> cards count
    /// @return Count of the <b>new</b> cards
    /// @note First, call the `setDeck` method to load the cards.
    [[nodiscard]] int new_cards_count() const noexcept;

    /// @brief Get <b>review</b> cards count
    /// @return Count of the <b>review</b> cards
    /// @note First, call the `setDeck` method to load the cards.
    [[nodiscard]] int review_cards_count() const noexcept;

    /// @brief Get <b>incorrect</b> cards count
    /// @return Count of the <b>incorrect</b> cards
    /// @note First, call the `setDeck` method to load the cards.
    [[nodiscard]] int incorrect_cards_count() const noexcept;

    /// @brief Get selected cards count
    /// @return Count of the selected cards
    [[nodiscard]] int selected_count() const noexcept;
};

} // namespace ui
