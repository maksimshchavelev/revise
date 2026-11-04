// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "wrappers/CardWrapper.hpp"            // for ui::Card
#include "wrappers/CreateCardDraftWrapper.hpp" // for ui::CreateCardDraft
#include "wrappers/DeckDraftWrapper.hpp"       // for ui::DeckDraft
#include "wrappers/DeckWrapper.hpp"            // for ui::Deck
#include "wrappers/UpdateCardDraftWrapper.hpp" // for ui::UpdateCardDraft
#include <QObject>                             // for QObject
#include <QtQmlIntegration/qqmlintegration.h>  // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/service/IDeckService.hpp>       // for core::IDeckService

namespace ui {

/// @brief UI wrapper for `core::IDeckService`
class DeckService final : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("DeckService is provided by C++")

  public:
    /**
     * @brief DeckService constructor
     * @param deck_service `core::IDeckService` interface
     * @param parent Pointer to parent `QObject`
     */
    explicit DeckService(core::IDeckService& deck_service, QObject* parent = nullptr);

    /// @copydoc core::IDeckService::create_deck
    Q_INVOKABLE void create_deck(const DeckDraft& deck);

    /// @copydoc core::IDeckService::remove_deck
    Q_INVOKABLE void remove_deck(int deck_id);

    /// @copydoc core::IDeckService::import_deck
    Q_INVOKABLE void import_deck(const QString& path);

    /// @copydoc core::IDeckService::export_deck
    Q_INVOKABLE void export_deck(int deck_id, const QString& path);

    /// @copydoc core::IDeckService::update_deck
    Q_INVOKABLE void update_deck(int deck_id, const DeckDraft& deck);

    /// @copydoc core::IDeckService::create_card
    Q_INVOKABLE void create_card(int deck_id, const CreateCardDraft& card);

    /// @copydoc core::IDeckService::remove_cards
    Q_INVOKABLE void remove_cards(const QVector<int>& ids);

    /// @copydoc core::IDeckService::update_card
    Q_INVOKABLE void update_card(const UpdateCardDraft& card);

    /// @brief Updates all cards with the specified IDs according to the `draft` template
    Q_INVOKABLE void update_cards(const QVector<int>& ids, const UpdateCardDraft& draft);

    /// @copydoc core::IDeckService::reset_cards
    Q_INVOKABLE void reset_cards(const QVector<int>& ids);

    /// @copydoc core::IDeckService::deck
    Q_INVOKABLE Deck deck(int deck_id) const;

    /// @copydoc core::IDeckService::card
    Q_INVOKABLE Card card(int card_id) const;

  private:
    core::IDeckService& m_deck_service; ///< Deck service
};

} // namespace ui
