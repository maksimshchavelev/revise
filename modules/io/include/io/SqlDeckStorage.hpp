// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Database.hpp"                 // for Database
#include "DatabaseExecutionContext.hpp" // for DatabaseExecutionContext
#include <core/IDeckStorage.hpp>        // for core::IDeckStorage

namespace io {

/**
 * @brief SQL database-based deck storage
 */
class SqlDeckStorage final : core::IDeckStorage {
  public:
    SqlDeckStorage(Database& db, DatabaseExecutionContext& context);

    /// @copydoc core::IDeckStorage::get_deck_summaries
    std::expected<QVector<core::DeckSummary>, QString> get_deck_summaries() override;

    /// @copydoc core::IDeckStorage::create_decks
    std::expected<void, QString> create_decks(const QVector<core::Deck>& decks) override;

    /// @copydoc core::IDeckStorage::update_decks
    std::expected<void, QString> update_decks(const QVector<core::Deck>& decks) override;

    /// @copydoc core::IDeckStorage::delete_decks
    std::expected<void, QString> delete_decks(const QVector<int>& ids) override;

    /// @copydoc core::IDeckStorage::fetch_decks
    std::expected<QVector<core::Deck>, QString> fetch_decks(const QVector<int>& ids) override;

    /// @copydoc core::IDeckStorage::fetch_decks
    std::expected<QVector<core::Deck>, QString> fetch_decks(const QVector<QString>& names) override;

    /// @copydoc core::IDeckStorage::update_cards
    std::expected<void, QString> update_cards(const QVector<core::Card>& cards) override;

    /// @copydoc core::IDeckStorage::insert_cards
    std::expected<void, QString> insert_cards(const QVector<core::Card>& cards) override;

    /// @copydoc core::IDeckStorage::fetch_cards
    std::expected<QVector<core::Card>, QString> fetch_cards(int deck_id) override;

    /// @copydoc core::IDeckStorage::fetch_cards
    std::expected<QVector<core::Card>, QString> fetch_cards(const QVector<int>& ids) override;

    /// @copydoc core::IDeckStorage::remove_cards
    std::expected<void, QString> remove_cards(const QVector<int>& ids) override;

  private:
    Database&                 m_db;      ///< Database reference
    DatabaseExecutionContext& m_context; ///< Execution context reference
};

} // namespace io