// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QAbstractListModel>
#include <core/IDeckService.hpp> // for core::IDeckService

namespace ui {

class DecksModel final : public QAbstractListModel {
  public:
    DecksModel(core::IDeckService& deck_service, QObject* parent = nullptr);

    enum DeckRoles {
        NameRole = Qt::UserRole + 1,
        TimeLimitRole,
        NewCardsRole,
        ConsolidateCardsRole,
        IncorrectCardsRole,
        DeckId,
        RepeatableToday
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

  private:
    core::IDeckService&        m_deck_service;
    QVector<core::DeckSummary> m_decks;

    /// Reload data
    void update();
};

} // namespace ui