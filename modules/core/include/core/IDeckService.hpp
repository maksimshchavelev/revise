// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Card.hpp"       // for Card
#include "Deck.hpp"       // for Deck
#include "Notifiable.hpp" // for Notifiable
#include <expected>       // for std::expected

namespace core {

/**
 * @brief Orchestrates all actions related to the deck and cards
 *
 * This class is useful if you want to delete, add, edit decks/cards, as well as import, export, and study them.
 */
class IDeckService : public Notifiable {
  public:
    SIGNAL(deck_created)    ///< Deck created
    SIGNAL(deck_removed)    ///< Deck removed
    SIGNAL(decks_updated)   ///< Any deck has been updated
    SIGNAL(import_started)  ///< Import started
    SIGNAL(import_finished) ///< Import finished
    SIGNAL(deck_imported)   ///< Deck imported
    SIGNAL(export_started)  ///< Export started
    SIGNAL(export_finished) ///< Export finished
    SIGNAL(deck_exported)   ///< Deck exported
    SIGNAL(card_created)    ///< Card created
    SIGNAL(cards_updated)   ///< Any card has been updated
    SIGNAL(card_removed)    ///< Card removed

    /**
     * @brief create_deck Create deck
     * @param deck Deck
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note If successful, the `deck_created` and `decks_updated` signals are emitted.
     */
    std::expected<void, QString> create_deck(const Deck& deck);

    /**
     * @brief Remove deck
     * @param deck_id ID of the deck
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note If successful, the `deck_removed` and `decks_updated` signals are emitted.
     */
    std::expected<void, QString> remove_deck(int deck_id);

    /**
     * @brief Import deck asynchronously
     * @param path Path to the deck
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note When importing begins, the `import_started` signal is emitted; when it ends, the `import_finished` signal
     * is emitted. If successful, the `deck_imported` and `decks_updated` signals are emitted.
     */
    std::expected<void, QString> import_deck_async(const QString& path);

    /**
     * @brief Exports the deck in `Revise` format (`rpkg`)
     * @param deck_id ID of the deck to be exported
     * @param path Where to save the result
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note When exporting begins, the `export_started` signal is emitted, and when it ends, the `export_finished`
     * signal is emitted. If successful, the `deck_exported` signal is emitted.
     */
    std::expected<void, QString> export_deck_async(int deck_id, const QString& path);

    /**
     * @brief Get information about the deck by id
     * @param deck_id Deck ID
     * @return `std::expected`, where `core::Deck` is located in case of success, and an error description in case of
     * failure
     */
    std::expected<Deck, QString> deck(int deck_id);

    /**
     * @brief Update deck
     * @param deck Deck
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note If successful, emits the `decks_updated` signal.
     */
    std::expected<void, QString> update_deck(const Deck& deck);

    /**
     * @brief Create a card
     * @param card Card
     * @return `std::expected` with `void` in case of success or with an error description in case of failure
     * @note If successful, emits the `card_created` and `cards_updated` signals.
     */
    std::expected<void, QString> create_card(const Card card);

    /**
     * @brief Get information about the card
     * @param id Card ID
     * @return `std::expected` with `core::Card` in case of success or with an error description in case of failure
     */
    std::expected<Card, QString> card(int id);

    /**
     * @brief Remove card
     * @param id ID of the card
     * @return `std::expected` with `core::Card` in case of success or with an error description in case of failure
     * @note If successful, emits the `card_removed` and `cards_updated` signals.
     */
    std::expected<void, QString> remove_card(int id);

    /**
     * @brief Update card
     * @param card Card
     * @return `std::expected` with `core::Card` in case of success or with an error description in case of failure
     * @note If successful, emits the `cards_updated` signal.
     */
    std::expected<void, QString> update_card(const Card& card);
};

} // namespace core
