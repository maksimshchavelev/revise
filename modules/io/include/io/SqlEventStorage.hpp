// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Database.hpp"                 // for Database
#include "DatabaseExecutionContext.hpp" // for DatabaseExecutionContext
#include <core/IEventStorage.hpp>       // for IEventStorage

namespace io {

/**
 * @brief SQL database-based event storage
 */
class SqlEventStorage final : core::IEventStorage {
  public:
    /**
     * @brief SqlEventStorage constructor
     * @param db `Database`
     * @param context The context in which all database queries will be executed
     * @note May throw an exception if a table creation or migration error occurred
     */
    SqlEventStorage(Database& db, DatabaseExecutionContext& context);

    /**
     * @see `core::IEventStorage` for details
     */
    std::expected<void, QString> record(const core::CardReviewEvent& event) override;

    /**
     * @see `core::IEventStorage` for details
     */
    std::expected<QVector<QVariant>, QString> fetch_synced(const core::EventType      event_type,
                                                           const core::EventSyncState sync_state,
                                                           int                        limit = 100) override;

    /**
     * @see `core::IEventStorage` for details
     */
    std::expected<void, QString> mark_synced_batch(QVector<int> ids, const core::EventSyncState sync_state) override;

    /**
     * @brief Method for database migration. Called by external code.
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    std::expected<void, QString> migrate();

  private:
    Database&                 m_db;      ///< Database reference
    DatabaseExecutionContext& m_context; ///< Execution context reference

    /**
     * @brief Creates events table if not exists
     */
    std::expected<void, QString> create_events_table();

    std::expected<void, QString> migrate_v1_to_v2();

    /**
     * @brief The structure describes the migration node—the version to which the migration is taking place—and the
     * function that performs the migration.
     */
    struct MigrationNode {
        int target_version;                                    ///< The version to which migration is taking place
        std::function<std::expected<void, QString>()> migrate; ///< The function that will be called for migration
    };

    QVector<MigrationNode> m_migration_chain; ///< Migration chain to migrate a table correctly
};

} // namespace io