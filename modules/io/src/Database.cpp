// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "io/Database.hpp"    // for Database header
#include <QCryptographicHash> // for QCryptographicHash
#include <QDebug>             // for QDebug, QWarning
#include <QDir>               // for QDir
#include <QFileInfo>          // for QFileInfo
#include <QSqlError>          // for QSqlError
#include <QSqlQuery>          // for QSqlQuery

namespace io {


// Public method
Database::Database(const QString& connection_name, const QString& db_name, QObject* parent) :
    QObject(parent), m_connection_name(connection_name) {

    QFileInfo info(db_name);
    QDir      dir = info.dir();

    if (!dir.mkpath(".")) {
        throw std::runtime_error(QString("Failed to create database directory: %1").arg(dir.absolutePath()).toStdString());
    }

    auto res = open_connection(db_name);
    if (!res.has_value()) {
        throw std::runtime_error(QString("Failed to open db connection. db_name = %1, connection_name = %2, error: %3")
                                     .arg(db_name)
                                     .arg(connection_name)
                                     .arg(res.error())
                                     .toStdString());
    }
}


// Public method
Database::~Database() {
    close_connection();
}


// Public method
std::expected<void, QString> Database::migrate() {
    QSqlQuery query(m_db);

    const auto calculate_checksum = [](const QByteArray& data) {
        return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
    };

    const auto migration_id = [](const QString& migration_filename) { return migration_filename.section('_', 0, 0).toInt(); };

    if (!query.exec("PRAGMA foreign_keys = ON")) {
        return std::unexpected(
            QString("Database::migrate(): failed to exec 'PRAGMA foreign_keys = ON' query: %1").arg(query.lastError().text()));
    }

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS schema_migrations (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            checksum TEXT NOT NULL,
            applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        );)")) {
        return std::unexpected(QString("Database::migrate(): failed to exec query to create 'schema_migrations' query: %1")
                                   .arg(query.lastError().text()));
    }

    if (!query.exec("SELECT id, name, checksum FROM schema_migrations ORDER BY id ASC")) {
        return std::unexpected(
            QString("Database::migrate(): failed to select applied migrations from 'schema_migrations' table: %1")
                .arg(query.lastError().text()));
    }

    int last_migration_id{0};

    while (query.next()) {
        last_migration_id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString checksum = query.value(2).toString();

        QFile file{":/sql_migrations/" + name};

        if (!file.open(QIODevice::ReadOnly)) {
            return std::unexpected(QString("Database::migrate(): failed to open migration file: %1").arg(name));
        }

        if (const auto raw_data = file.readAll(); calculate_checksum(raw_data) != checksum) {
            return std::unexpected(QString("Database::migrate(): change of migration file detected: %1").arg(name));
        }
    }

    QDir dir{":/sql_migrations"};
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList() << "*.sql");

    for (const auto& migration : dir.entryInfoList()) {
        const auto id = migration_id(migration.fileName());

        if (id <= last_migration_id) {
            continue;
        }

        QFile file(migration.absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly)) {
            return std::unexpected(QString("Database::migrate(): failed to open migration file: %1").arg(migration.fileName()));
        }

        const auto raw_data = file.readAll();

        const auto checksum = calculate_checksum(raw_data);
        const auto sql_text = QString::fromUtf8(raw_data);

        if (const auto res = begin_transaction(); !res) {
            return std::unexpected(res.error());
        }

        for (const auto& statement : sql_text.split("-- revise:statement")) {
            if (!query.exec(statement)) {
                m_db.rollback();
                return std::unexpected(QString("Database::migrate(): failed to apply statement '%1' in migration '%2': %3")
                                           .arg(statement, migration.fileName(), query.lastError().text()));
            }
        }

        query.prepare("INSERT INTO schema_migrations (id, name, checksum) VALUES (:id, :name, :checksum)");
        query.bindValue(":id", id);
        query.bindValue(":name", migration.fileName());
        query.bindValue(":checksum", checksum);

        if (!query.exec()) {
            m_db.rollback();
            return std::unexpected(
                QString("Database::migrate(): failed to update 'schema_migrations' table after applying migration: %1")
                    .arg(query.lastError().text()));
        }

        if (const auto res = commit_transaction(); !res) {
            m_db.rollback();
            return std::unexpected(res.error());
        }

        qDebug() << QString("The new database migration '%1' was successfully completed").arg(migration.fileName());
    }

    return {};
}


// Public method
std::expected<void, QString> Database::begin_transaction() {
    if (!m_db.transaction()) {
        return std::unexpected(m_db.lastError().text());
    }

    return {};
}


// Public method
std::expected<void, QString> Database::commit_transaction() {
    if (!m_db.commit()) {
        return std::unexpected(m_db.lastError().text());
    }

    return {};
}


// Public method
std::expected<void, QString> Database::rollback_transaction() {
    if (!m_db.rollback()) {
        return std::unexpected(m_db.lastError().text());
    }

    return {};
}


// Public method
QSqlDatabase& Database::raw_db() noexcept {
    return m_db;
}


// Public method
QString Database::last_error_text() const {
    return m_db.lastError().text();
}


// ==================== PRIVATE SECTION ====================

// Private method
std::expected<void, QString> Database::open_connection(const QString& db_name) {
    if (QSqlDatabase::contains(m_connection_name)) {
        QSqlDatabase::removeDatabase(m_connection_name);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connection_name);
    m_db.setDatabaseName(db_name);

    if (!m_db.open()) {
        return std::unexpected(m_db.lastError().text());
    }

    return {};
}


// Private method
void Database::close_connection() {
    if (m_db.isOpen()) {
        m_db.close();
    }

    QSqlDatabase::removeDatabase(m_connection_name);
}

} // namespace io
