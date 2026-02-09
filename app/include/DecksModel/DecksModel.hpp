// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A model that stores a list of decks. Can be used in QML.

#pragma once

#include <Database/Database.hpp>       // for Database
#include <DeckService/DeckService.hpp> // for DeckService
#include <QAbstractListModel>          // for QAbstractListModel
// #include <StudyController/StudyController.hpp> // for StudyController

namespace revise {

class DecksModel : public QAbstractListModel {
    Q_OBJECT

  public:
    DecksModel(DeckService& service);

    enum DeckRoles {
        NameRole = Qt::UserRole + 1,
        TimeLimitRole,
        NewCardsRole,
        ConsolidateCardsRole,
        IncorrectCardsRole,
        DeckId,
        RepeatableToday
    };

    int                    rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant               data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update();

  private:
    DeckService&         m_deck_service;
    QVector<DeckSummary> m_decks;
};

} // namespace revise
