// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <../../core/include/core/service/IStudyService.hpp>
#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/service/IDeckService.hpp>      // for core::IDeckService

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
        ConsolidateCardsRole,
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

    /// @brief Reload data
    void update();
};

} // namespace ui