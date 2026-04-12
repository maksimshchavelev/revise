// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/Database.hpp" // for Database header
#include <QDateTime>       // for QDateTime
#include <QDebug>          // for QDebug, QWarning
#include <QDir>            // for QDir
#include <QFileInfo>       // for QFileInfo
#include <QSqlError>       // for QSqlError
#include <QSqlQuery>       // for QSqlQuery

namespace io {


// Public method
Database::Database(const QString& connection_name, const QString& db_name, QObject* parent) :
    QObject(parent), m_connection_name(connection_name) {

    QFileInfo info(db_name);
    QDir      dir = info.dir();

    if (!dir.mkpath(".")) {
        throw std::runtime_error(
            QString("Failed to create database directory: %1").arg(dir.absolutePath()).toStdString());
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
std::expected<void, QString> Database::init_db() {
    QSqlQuery streak_query(m_db);
    QSqlQuery decks_query(m_db);
    QSqlQuery cards_query(m_db);
    QSqlQuery fk_query(m_db);
    QSqlQuery schema_versions_query(m_db);

    // Enable foreign key support
    if (!fk_query.exec("PRAGMA foreign_keys = ON")) {
        return std::unexpected(
            QString("Failed to exec 'PRAGMA foreign_keys = ON' query: %1").arg(fk_query.lastError().text()));
    }

    // Insert streak
    // streak_query.prepare("INSERT OR IGNORE INTO streak (id, value, last_updated) VALUES (1, :val, :date)");
    // streak_query.bindValue(":val", 0);
    // streak_query.bindValue(":date", QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd HH:mm:ss"));

    // if (!streak_query.exec()) {
    //     return std::unexpected(QString("Failed to exec streak init query: %1").arg(streak_query.lastError().text()));
    // }

    // Create DECKS table
    decks_query.prepare(R"(
        CREATE TABLE IF NOT EXISTS decks (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE,
            description TEXT,
            time_limit INTEGER DEFAULT 0,
            new_limit INTEGER DEFAULT 30,
            consolidate_limit INTEGER DEFAULT 30,
            incorrect_limit INTEGER DEFAULT 30)
    )");

    if (!decks_query.exec()) {
        return std::unexpected(QString("Failed to exec decks init query: %1").arg(decks_query.lastError().text()));
    }

    // Create CARDS table
    cards_query.prepare(R"(
        CREATE TABLE IF NOT EXISTS cards (
            id INTEGER PRIMARY KEY,
            deck_id INTEGER NOT NULL,
            front TEXT NOT NULL,
            back TEXT NOT NULL,
            state INTEGER DEFAULT 0,             -- 0=new, 1=consolidate, 2=incorrect (relearn)
            difficulty REAL DEFAULT 2.5,         -- SM-2, 1.0–5.0
            interval INTEGER DEFAULT 0,          -- interval between repeats in days
            next_review DATETIME DEFAULT CURRENT_TIMESTAMP,
            incorrect_streak INTEGER DEFAULT 0,  -- number of consecutive incorrect answers
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(deck_id) REFERENCES decks(id) ON DELETE CASCADE,
            UNIQUE(deck_id, front)
    ))");

    if (!cards_query.exec()) {
        return std::unexpected(QString("Failed to exec cards init query: %1").arg(cards_query.lastError().text()));
    }

    // create schema_versions table
    if (!schema_versions_query.exec(R"(
        CREATE TABLE IF NOT EXISTS schema_versions (
            table_name TEXT PRIMARY KEY,
            version INTEGER DEFAULT 1
        )
    )")) {
        return std::unexpected(
            QString("Failed to create 'schema_versions' table: %1").arg(schema_versions_query.lastError().text()));
    }

    if (auto migration_res = migrate(); !migration_res.has_value()) {
        return std::unexpected(QString("Failed to migrate schema: %1").arg(migration_res.error()));
    }

    qDebug() << "Migrations applied";

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


// Private method
std::expected<int, QString> Database::get_db_schema_version() const {
    if (!m_db.isOpen()) {
        return std::unexpected("DB is not opened");
    }

    QSqlQuery q(m_db);
    if (!q.exec("PRAGMA user_version")) {
        return std::unexpected(QString("Failed to get schema version: %1").arg(q.lastError().text()));
    }

    if (q.next()) {
        return q.value(0).toInt();
    }

    return 0; // default
}


// Private method
std::expected<void, QString> Database::set_db_schema_version(int version) {
    if (!m_db.isOpen()) {
        return std::unexpected("DB is not opened");
    }

    QSqlQuery q(m_db);

    if (!q.exec(QString("PRAGMA user_version = %1").arg(version))) {
        return std::unexpected(QString("Failed to set schema version: %1").arg(q.lastError().text()));
    }

    return {};
}


// Private method
std::expected<void, QString> Database::migrate() {
    // Migrate to 2
    if (auto version = get_db_schema_version(); version.has_value() && version.value() <= 1) {
        if (auto res = migrage_1_to_2(); !res.has_value()) {
            return std::unexpected(QString("Failed to migrate from schema 1 to 2: %1").arg(res.error()));
        }
    } else if (!version.has_value()) {
        return std::unexpected(QString("Failed to get schema version: %1").arg(version.error()));
    }

    return {};
}


// Private method
std::expected<void, QString> Database::migrage_1_to_2() {
    if (!m_db.isOpen()) {
        return std::unexpected("DB is not opened");
    }

    if (!m_db.transaction()) {
        return std::unexpected("Failed to begin transaction");
    }

    QSqlQuery q(m_db);

    // Create events table
    q.prepare(R"(
        CREATE TABLE IF NOT EXISTS events (
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL,            -- type of event (for example, card_review)
            created_at DATETIME NOT NULL,
            payload TEXT NOT NULL,         -- payload (for example, JSON description)
            synced INTEGER DEFAULT 0       -- synced with server?
        )
    )");

    if (!q.exec()) {
        m_db.rollback();
        return std::unexpected(QString("Failed to create table 'events'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    // Update decks table
    if (!q.exec("ALTER TABLE decks ADD COLUMN global_id INTEGER DEFAULT 0")) {
        m_db.rollback();
        return std::unexpected(QString("Failed to update table 'decks'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    if (!q.exec("ALTER TABLE decks ADD COLUMN source TEXT DEFAULT 'local'")) /* local or remote */ {
        m_db.rollback();
        return std::unexpected(QString("Failed to update table 'decks'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    if (!q.exec("ALTER TABLE decks ADD COLUMN version INTEGER DEFAULT 0") /* version of the deck */) {
        m_db.rollback();
        return std::unexpected(QString("Failed to update table 'decks'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    // Update cards table
    if (!q.exec("ALTER TABLE cards ADD COLUMN global_id INTEGER DEFAULT 0")) {
        m_db.rollback();
        return std::unexpected(QString("Failed to update table 'cards'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    if (!q.exec("ALTER TABLE cards ADD COLUMN deck_global_id INTEGER DEFAULT 0")) {
        m_db.rollback();
        return std::unexpected(QString("Failed to update table 'cards'. Query: %1, cause: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    if (auto res = set_db_schema_version(m_app_schema_version); !res.has_value()) {
        m_db.rollback();
        return std::unexpected(QString("Failed to set schema version, cause: %1").arg(res.error()));
    }

    if (!m_db.commit()) {
        m_db.rollback();
        return std::unexpected("Failed to commit transaction");
    }

    return {}; // success
}

} // namespace io
