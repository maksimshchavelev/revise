// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Card.hpp"              // for ui::Card wrapper
#include "Deck.hpp"              // for ui::Deck wrapper
#include <QObject>               // for QObject
#include <core/IDeckService.hpp> // for core::IDeckService

namespace ui {

/**
 * @brief UI wrapper for `core::IDeckService`
 */
class DeckService final : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief DeckService constructor
     * @param deck_service `core::IDeckService` interface
     * @param parent Pointer to parent `QObject1`
     */
    DeckService(core::IDeckService& deck_service, QObject* parent = nullptr);

    /// @copydoc core::IDeckService::create_deck
    Q_INVOKABLE void create_deck(const Deck& deck);

    /// @copydoc core::IDeckService::remove_deck
    Q_INVOKABLE void remove_deck(int deckId);

    /// @copydoc core::IDeckService::import_deck_async
    Q_INVOKABLE void import_deck_async(const QString& path);

    /// @copydoc core::IDeckService::import_deck_async
    Q_INVOKABLE void export_deck_async(int deckId, const QString& path);

    /// @copydoc core::IDeckService::update_deck
    Q_INVOKABLE void update_deck(const Deck& deck);

    /// @copydoc core::IDeckService::create_card
    Q_INVOKABLE void create_card(const Card& card);

    /// @copydoc core::IDeckService::remove_card
    Q_INVOKABLE void remove_card(int id);

    /// @copydoc core::IDeckService::update_card
    Q_INVOKABLE void update_card(const Card& deck);

    /// @copydoc core::IDeckService::deck
    Q_INVOKABLE Deck deck(int deckId);

  private:
    core::IDeckService& m_deck_service; ///< Deck service
};

} // namespace ui
