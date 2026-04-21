// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Database.hpp"                 // for Database
#include "DatabaseExecutionContext.hpp" // for DatabaseExecutionContext
#include <core/IStreakStorage.hpp>      // for core::IStreakStorage

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
    Result<void> save(const core::Streak streak) override;

    /// @copydoc core::IStreakStorage::read
    Result<core::Streak> read() const override;

    /**
     * @brief Method for database migration. Called by external code.
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    std::expected<void, QString> migrate();

  private:
    Database&                 m_db;
    DatabaseExecutionContext& m_context;

    /// @brief Creates streak table if not exists
    std::expected<void, QString> create_streak_table();

    /// @brief Make error from sql error
    static Error from_sql(const QSqlError& error, QString context = {});
};

} // namespace io