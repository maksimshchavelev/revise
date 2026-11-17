// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>    // for QDateTime
#include <QObject>      // for QObject
#include <QSqlDatabase> // for QSqlDatabase
#include <expected>     // for std::expected

namespace io {

/**
 * @brief Lightweight SQL driver / helper.
 *
 * Responsible for managing QSqlDatabase connection, migrations, transactions and
 * small utility helpers. It intentionally does NOT implement domain CRUD.
 */
class Database : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Construct a Database object and open the connection.
     * @param connection_name Unique connection name for QSqlDatabase.
     * @param db_name File path to sqlite file. If empty, an in-memory DB is used.
     * @param parent QObject parent.
     */
    explicit Database(const QString& connection_name = "revise_main",
                      const QString& db_name = "revise.db",
                      QObject*       parent = nullptr);

    ~Database() override;

    /**
     * @brief Read database migrations from `migrations` resource directory and apply them
     * @return std::expected<void, QString> empty on success, error string otherwise.
     */
    std::expected<void, QString> migrate();

    /**
     * @brief Start a transaction on the underlying QSqlDatabase.
     */
    std::expected<void, QString> begin_transaction();

    /**
     * @brief Commit current transaction.
     */
    std::expected<void, QString> commit_transaction();

    /**
     * @brief Rollback current transaction.
     */
    std::expected<void, QString> rollback_transaction();

    /**
     * @brief Get raw QSqlDatabase reference. Use with caution.
     */
    QSqlDatabase& raw_db() noexcept;

    /**
     * @brief Return last error text from the connection.
     */
    [[nodiscard]] QString last_error_text() const;

  signals:
    /**
     * @brief Emitted when a non-fatal internal error occurs (for logging/debug).
     */
    void error_occurred(const QString& what);

  private:
    QString      m_connection_name;
    QSqlDatabase m_db;

    std::expected<void, QString> open_connection(const QString& db_name);
    void                         close_connection();
};

} // namespace revise
