// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "core/Card.hpp"                    // for Card
#include "core/Deck.hpp"                    // for Deck
#include "core/Notifiable.hpp"              // for Notifiable
#include "core/search/CardSearchFilter.hpp" // for CardFilterChain
#include <QFuture>                          // for QFuture
#include <expected>                         // for std::expected

namespace core {

/**
 * @brief Orchestrates all actions related to the deck and cards
 * @details This class is useful if you want to delete, add, edit decks/cards, as well as import and export them
 */
class IDeckService : public Notifiable {
  public:
    /// @brief Stores IDeckService error
    struct Error {
        enum class Kind {
            NotFound,          ///< Requested entity does not exist
            AlreadyExists,     ///< Entity already exists
            ValidationFailed,  ///< Input data is invalid
            ImportFailed,      ///< Import operation failed
            ExportFailed,      ///< Export operation failed
            UnsupportedFormat, ///< Unsupported import/export format
            AccessDenied,      ///< Operation is forbidden
            Busy,              ///< Another operation is already running
            InternalError      ///< Unexpected internal failure
        };

        Kind    kind{Kind::InternalError};
        QString message;

        static Error not_found(QString message) {
            return Error{.kind = Kind::NotFound, .message = std::move(message)};
        }

        static Error already_exists(QString message) {
            return Error{.kind = Kind::AlreadyExists, .message = std::move(message)};
        }

        static Error validation_failed(QString message) {
            return Error{.kind = Kind::ValidationFailed, .message = std::move(message)};
        }

        static Error import_failed(QString message) {
            return Error{.kind = Kind::ImportFailed, .message = std::move(message)};
        }

        static Error export_failed(QString message) {
            return Error{.kind = Kind::ExportFailed, .message = std::move(message)};
        }

        static Error unsupported_format(QString message) {
            return Error{.kind = Kind::UnsupportedFormat, .message = std::move(message)};
        }

        static Error access_denied(QString message) {
            return Error{.kind = Kind::AccessDenied, .message = std::move(message)};
        }

        static Error busy(QString message) {
            return Error{.kind = Kind::Busy, .message = std::move(message)};
        }

        static Error internal_error(QString message) {
            return Error{.kind = Kind::InternalError, .message = std::move(message)};
        }
    };

    template <typename T> using Result = std::expected<T, Error>;

    /// @brief Used as a draft for the deck to be created or updated
    struct DeckDraft {
        QString name;
        QString description;
        int     new_limit{0};
        int     review_limit{0};
        int     incorrect_limit{0};
        int     time_limit{0};
    };

    /// @brief Used as a draft for the card to be created
    struct CreateCardDraft {
        QString front;
        QString back;
    };

    /// @brief Used as a draft for the card to be updated
    struct UpdateCardDraft : CreateCardDraft {
        float difficulty{0.0f};
    };

    EVENT(deck_created, Deck deck)  ///< Deck created. Stores created deck
    EVENT(deck_removed, Deck deck)  ///< Deck removed. Stores removed deck
    EVENT(deck_updated, Deck deck)  ///< Deck updated
    EVENT(deck_imported, Deck deck) ///< Deck imported (upon successful import). Stores imported deck
    EVENT(deck_exported, Deck deck) ///< Deck exported (upon successful export). Stores exported deck

    EVENT(card_created, Card card) ///< Card created. Stores created card
    EVENT(card_removed, Card card) ///< Card removed. Stores removed card
    EVENT(card_updated, Card card) ///< Card updated. Stores updated card

    EVENT(import_failed, Error error) ///< Import failed
    EVENT(export_failed, Error error) ///< Export failed

    EVENT(deck_create_failed, Error error) ///< Failed to create deck
    EVENT(deck_update_failed, Error error) ///< Failed to update deck
    EVENT(deck_remove_failed, Error error) ///< Failed to remove deck

    EVENT(card_create_failed, Error error) ///< Failed to create card
    EVENT(card_update_failed, Error error) ///< Failed to update card
    EVENT(card_remove_failed, Error error) ///< Failed to remove card

    EVENT(import_finished) ///< Import finished with success or failure
    EVENT(export_finished) ///< Export finished with success or failure

    virtual ~IDeckService() = default;

    /**
     * @brief Create deck
     * @param deck Deck draft
     * @note Emits `deck_created` event if success
     */
    virtual QFuture<Result<void>> create_deck(const DeckDraft& deck) = 0;

    /**
     * @brief Remove deck by ID
     * @param deck_id Deck ID
     * @note Emits `deck_removed` event if success
     */
    virtual QFuture<Result<void>> remove_deck(Deck::id_type deck_id) = 0;

    /**
     * @brief Import deck
     * @param path Path to the deck
     * @note Emits `deck_imported` if deck successfully imported. Emits `import_finished` in any case
     */
    virtual QFuture<Result<void>> import_deck(const QString& path) = 0;

    /**
     * @brief Export deck
     * @param deck_id Deck id
     * @param path Path to the deck
     * @note Emits `deck_exported` if deck successfully exported. Emits `export_finished` in any case
     */
    virtual QFuture<Result<void>> export_deck(Deck::id_type deck_id, const QString& path) = 0;

    /**
     * @brief Fetch deck by ID
     * @param deck_id Deck ID
     * @return Fetched deck
     */
    virtual QFuture<Result<Deck>> deck(Deck::id_type deck_id) const = 0;

    /**
     * @brief Update deck
     * @param deck_id Deck ID
     * @param deck Deck
     * @note Emits `deck_updated` event if success
     */
    virtual QFuture<Result<void>> update_deck(Deck::id_type deck_id, const DeckDraft& deck) = 0;

    /**
     * @brief Create a card
     * @param card Card
     * @note Emits `card_created` event if success
     */
    virtual QFuture<Result<void>> create_card(const CreateCardDraft& card) = 0;

    /**
     * @brief Fetch card
     * @param id Card ID
     * @return Fetched card
     */
    virtual QFuture<Result<Card>> card(Card::id_type id) const = 0;

    /**
     * @brief Remove card
     * @param id ID of the card
     * @note Emits `card_removed` event if success
     */
    virtual std::expected<void, QString> remove_card(Card::id_type id) = 0;

    /**
     * @brief Update card
     * @param card Card
     * @note Emits `card_updated` event if success
     */
    virtual QFuture<Result<void>> update_card(const UpdateCardDraft& card) = 0;

    /**
     * @brief Get information about each deck
     * @return QVector of deck summary
     */
    virtual QFuture<Result<QVector<DeckSummary>>> deck_summaries() const = 0;

    /**
     * @brief Get a list of all cards in specified deck
     * @param deck_id Deck ID
     * @return QVector of cards
     */
    virtual QFuture<Result<QVector<Card>>> cards(Deck::id_type deck_id) const = 0;

    /**
     * @brief Search cards in the database
     * @param filters Search filters
     * @return QVector of cards
     */
    virtual QFuture<Result<QVector<Card>>> search_cards(const CardFilterChain& filters) const = 0;
};

} // namespace core
