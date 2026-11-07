// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>                 // for QAbstractListModel
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/service/IDeckService.hpp>      // for core::IDeckService
#include <core/service/IStudyService.hpp>     // for core::IStudyService

namespace ui {

class DecksModel final : public QAbstractListModel {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("DecksModel is provided by C++")

  public:
    DecksModel(core::IDeckService& deck_service, core::IStudyService& study_service, QObject* parent = nullptr);

    enum DeckRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        TimeLimitRole,
        NewCardsRole,
        ReviewCardsRole,
        IncorrectCardsRole,
        DeckId,
        RepeatableToday,
        IsSpecialRole ///< If true, this is a special deck used to add a new deck
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

  private:
    core::IDeckService&        m_deck_service;
    core::IStudyService&       m_study_service;
    QVector<core::DeckSummary> m_decks;

    /// @brief Reload all decks from the service (used on construction and as a fallback)
    void reload();

    /// @brief Handle deck_created / deck_imported event
    void on_deck_created(const core::Deck& deck);

    /// @brief Handle deck_removed event
    void on_deck_removed(const core::Deck& deck);

    /// @brief Handle deck_updated / deck_exported event
    void on_deck_updated(const core::Deck& deck);

    /// @brief Handle card_created event
    void on_card_created(const core::Card& card);

    /// @brief Handle cards_removed event
    void on_cards_removed(const QVector<core::Card>& cards);

    /// @brief Handle cards_updated event
    void on_cards_updated(const QVector<core::Card>& cards);

    /// @brief Handle training_finished event
    void on_training_finished();

    /// @brief Find row index of a deck by its ID
    /// @param deckId The local ID of the deck
    /// @return Row index, or -1 if the deck is not found
    [[nodiscard]] int find_row(int deckId) const noexcept;

    /// @brief Update counters for all decks affected by the given card events
    /// @param affectedDeckIds Set of deck IDs whose card composition may have changed
    void refresh_decks(const QSet<int>& affectedDeckIds);

    /// @brief Insert a new deck summary in the correct sorted position
    /// @param summary The deck summary to insert
    void insert_deck_summary(const core::DeckSummary& summary);

    /// @brief Update an existing deck summary at the given row
    /// @param row Row index
    /// @param summary New summary data
    void update_deck_summary(int row, const core::DeckSummary& summary);
};

} // namespace ui
