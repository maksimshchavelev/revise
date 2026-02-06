// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

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
     * @param db_file_path Path to sqlite file. If empty, an in-memory DB is used.
     * @param parent QObject parent.
     */
    explicit Database(const QString& connection_name = "revise_main",
                      const QString& db_name = "revise.db",
                      QObject*       parent = nullptr);

    ~Database() override;

    /**
     * @brief Ensure schema exists
     * @return std::expected<void, QString> empty on success, error string otherwise.
     */
    std::expected<void, QString> init_db();

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
    QString last_error_text() const;

    /**
     * @brief Get a version of the database schema
     * @return Version (`0` by default) or error description
     */
    std::expected<int, QString> get_db_schema_version() const;

    /**
     * @brief Set the database schema version
     * @param version Schema version
     * @return `std::expected<void, QString>`
     */
    std::expected<void, QString> set_db_schema_version(int version);

  signals:
    /**
     * @brief Emitted when a non-fatal internal error occurs (for logging/debug).
     */
    void error_occurred(const QString& what);

  private:
    QString      m_connection_name;
    QSqlDatabase m_db;

    const int m_app_schema_version{2}; ///< Required for migrations

    std::expected<void, QString> open_connection(const QString& db_name);
    void                         close_connection();

    /**
     * @brief Migrates the database from the latest version to the current one
     * @return std::expected<void, QString>
     */
    std::expected<void, QString> migrate();

    /**
     * @brief Migrates database from version 1 to 2
     * @return std::expected<void, QString>
     * @note Updates `m_app_schema_version` if success
     */
    std::expected<void, QString> migrage_1_to_2();
};

} // namespace revise
