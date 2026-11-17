// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Database.hpp"                    // for Database
#include "DatabaseExecutionContext.hpp"    // for DatabaseExecutionContext
#include <core/storage/IStreakStorage.hpp> // for core::IStreakStorage

namespace io {

class SqlStreakStorage final : public core::IStreakStorage {
  public:
    /**
     * @brief SqlStreakStorage constructor
     * @param db `Database`
     * @param context The context in which all database queries will be executed
     * @note May throw an exception if a table creation error occurred
     */
    SqlStreakStorage(Database& db, DatabaseExecutionContext& context);

    /// @copydoc core::IStreakStorage::save
    [[nodiscard]] Result<void> save(core::Streak streak) override;

    /// @copydoc core::IStreakStorage::read
    [[nodiscard]] Result<core::Streak> read() const override;

  private:
    Database&                 m_db;
    DatabaseExecutionContext& m_context;

    /// @brief Make error from sql error
    static Error from_sql(const QSqlError& error, QString context = {});
};

} // namespace io