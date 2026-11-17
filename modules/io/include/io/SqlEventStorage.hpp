// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Database.hpp"                   // for Database
#include "DatabaseExecutionContext.hpp"   // for DatabaseExecutionContext
#include <core/storage/IEventStorage.hpp> // fore core::IEventStorage

namespace io {

/**
 * @brief SQL database-based event storage
 */
class SqlEventStorage final : public core::IEventStorage {
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
    std::expected<QVector<QVariant>, QString> fetch_synced(core::EventType      event_type,
                                                           core::EventSyncState sync_state,
                                                           int                  limit) override;

    /**
     * @see `core::IEventStorage` for details
     */
    std::expected<void, QString> mark_synced_batch(QVector<int> ids, core::EventSyncState sync_state) override;

  private:
    Database&                 m_db;      ///< Database reference
    DatabaseExecutionContext& m_context; ///< Execution context reference
};

} // namespace io