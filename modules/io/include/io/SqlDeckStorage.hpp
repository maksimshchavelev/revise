// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Database.hpp"                  // for Database
#include "DatabaseExecutionContext.hpp"  // for DatabaseExecutionContext
#include <core/storage/IDeckStorage.hpp> // for core::IDeckStorage

namespace io {

/// @brief SQL database-based deck storage
class SqlDeckStorage final : public core::IDeckStorage {
  public:
    SqlDeckStorage(Database& db, DatabaseExecutionContext& context);

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Deck>> create_decks(const QVector<core::Deck>& decks) override;

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Deck>> update_decks(const QVector<core::Deck>& decks) override;

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Deck>> remove_decks(const QVector<core::Deck::id_type>& ids) override;

    /// @copydoc core::IDeckStorage::create_decks
    [[nodiscard]] Result<QVector<core::Deck>> fetch_decks(const QVector<core::Deck::id_type>& ids) const override;

    /// @copydoc core::IDeckStorage::create_decks
    [[nodiscard]] Result<QVector<core::Deck>> fetch_decks() const override;

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Card>> update_cards(const QVector<core::Card>& cards) override;

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Card>> create_cards(const QVector<core::Card>& cards) override;

    /// @copydoc core::IDeckStorage::create_decks
    [[nodiscard]] Result<QVector<core::Card>> fetch_cards(core::Deck::id_type deck_id) const override;

    /// @copydoc core::IDeckStorage::create_decks
    [[nodiscard]] Result<QVector<core::Card>> fetch_cards(const QVector<core::Card::id_type>& ids) const override;

    /// @copydoc core::IDeckStorage::create_decks
    [[nodiscard]] Result<QVector<core::Card>> fetch_cards() const override;

    /// @copydoc core::IDeckStorage::create_decks
    Result<QVector<core::Card>> remove_cards(const QVector<core::Card::id_type>& ids) override;

  private:
    /// @brief Make `Error` with message from `QSqlError`
    static Error from_sql_error(const QSqlError& error, const QString& message = {});

    Database&                 m_db;      ///< Database reference
    DatabaseExecutionContext& m_context; ///< Execution context reference
};

} // namespace io