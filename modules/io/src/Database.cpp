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

    const auto set_foreign_keys = [&](const bool enabled) -> std::expected<void, QString> {
        if (const QString pragma =
                enabled ? QStringLiteral("PRAGMA foreign_keys = ON") : QStringLiteral("PRAGMA foreign_keys = OFF");
            !query.exec(pragma)) {
            return std::unexpected(
                QString("Database::migrate(): failed to exec '%1' query: %2").arg(pragma, query.lastError().text()));
        }

        return {};
    };

    const auto rollback_and_restore_foreign_keys = [&](QString error) -> std::unexpected<QString> {
        query.finish();

        if (!m_db.rollback()) {
            error += QString("; failed to rollback transaction: %1").arg(m_db.lastError().text());
        }

        if (const auto res = set_foreign_keys(true); !res) {
            error += QString("; %1").arg(res.error());
        }

        return std::unexpected(std::move(error));
    };

    if (const auto res = set_foreign_keys(true); !res) {
        return std::unexpected(res.error());
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
        const QString name = query.value(1).toString();
        const QString checksum = query.value(2).toString();

        QFile file{":/sql_migrations/" + name};

        if (!file.open(QIODevice::ReadOnly)) {
            return std::unexpected(QString("Database::migrate(): failed to open migration file: %1").arg(name));
        }

        if (const auto raw_data = file.readAll(); calculate_checksum(raw_data) != checksum) {
            return std::unexpected(QString("Database::migrate(): change of migration file detected: %1").arg(name));
        }
    }

    query.finish();

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

        if (const auto res = set_foreign_keys(false); !res) {
            return std::unexpected(res.error());
        }

        if (const auto res = begin_transaction(); !res) {
            QString error = res.error();

            if (const auto restore = set_foreign_keys(true); !restore) {
                error += QString("; %1").arg(restore.error());
            }

            return std::unexpected(std::move(error));
        }

        for (const auto& statement : sql_text.split("-- revise:statement")) {
            if (!query.exec(statement)) {
                const auto error = QString("Database::migrate(): failed to apply statement '%1' in migration '%2': %3")
                                       .arg(statement, migration.fileName(), query.lastError().text());

                return rollback_and_restore_foreign_keys(error);
            }
        }

        query.prepare("INSERT INTO schema_migrations (id, name, checksum) VALUES (:id, :name, :checksum)");
        query.bindValue(":id", id);
        query.bindValue(":name", migration.fileName());
        query.bindValue(":checksum", checksum);

        if (!query.exec()) {
            const auto error =
                QString("Database::migrate(): failed to update 'schema_migrations' table after applying migration: %1")
                    .arg(query.lastError().text());

            return rollback_and_restore_foreign_keys(error);
        }

        if (!query.exec("PRAGMA foreign_key_check")) {
            const auto error = QString("Database::migrate(): failed to execute foreign key check after migration '%1': %2")
                                   .arg(migration.fileName(), query.lastError().text());

            return rollback_and_restore_foreign_keys(error);
        }

        QStringList foreign_key_violations;

        while (query.next()) {
            const QString table = query.value(0).toString();
            const QString rowid = query.value(1).isNull() ? QStringLiteral("NULL") : query.value(1).toString();
            const QString parent = query.value(2).toString();
            const int     foreign_key_index = query.value(3).toInt();

            foreign_key_violations.append(QString("table='%1', rowid=%2, parent='%3', foreign_key_index=%4")
                                              .arg(table, rowid, parent)
                                              .arg(foreign_key_index));
        }

        query.finish();

        if (!foreign_key_violations.isEmpty()) {
            const auto error = QString("Database::migrate(): foreign key violations detected after migration '%1': %2")
                                   .arg(migration.fileName(), foreign_key_violations.join("; "));

            return rollback_and_restore_foreign_keys(error);
        }

        if (const auto res = commit_transaction(); !res) {
            return rollback_and_restore_foreign_keys(res.error());
        }

        if (const auto res = set_foreign_keys(true); !res) {
            return std::unexpected(
                QString("Database::migrate(): migration '%1' was committed, but failed to restore foreign keys: %2")
                    .arg(migration.fileName(), res.error()));
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
