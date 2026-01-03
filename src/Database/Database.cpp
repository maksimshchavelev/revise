// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Class controlling the streak

#include <Database/Database.hpp>
#include <QDateTime> // for QDateTime
#include <QDebug>    // for QDebug, QWarning
#include <QDir>      // for QDir
#include <QFileInfo> // for QFileInfo
#include <QSqlError> // for QSqlError
#include <QSqlQuery> // for QSqlQuery

namespace revise {


// Public method
Database::Database(const QString& connection_name, const QString& db_name, QObject* parent) :
    QObject(parent), m_connection_name(connection_name) {
    auto res = open_connection(db_name);
    if (!res.has_value()) {
        emit error_occurred(res.error());
    } else {
        auto init_res = init_db();
        if (!init_res.has_value()) {
            emit error_occurred(init_res.error());
        }
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

    // Enable foreign key support
    if (!fk_query.exec("PRAGMA foreign_keys = ON")) {
        return std::unexpected(fk_query.lastError().text());
    }

    // Create STREAK table
    streak_query.prepare(R"(
        CREATE TABLE IF NOT EXISTS streak (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            value INTEGER DEFAULT 0,
            last_updated DATETIME DEFAULT (DATETIME('now', 'localtime', '-1 day')))
    )");

    if (!streak_query.exec()) {
        return std::unexpected(streak_query.lastError().text());
    }

    // Insert streak
    streak_query.prepare("INSERT OR IGNORE INTO streak (id, value, last_updated) VALUES (1, :val, :date)");
    streak_query.bindValue(":val", 0);
    streak_query.bindValue(":date", QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd HH:mm:ss"));

    if (!streak_query.exec()) {
        return std::unexpected(streak_query.lastError().text());
    }

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
        return std::unexpected(decks_query.lastError().text());
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
        return std::unexpected(cards_query.lastError().text());
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
QSqlDatabase &Database::raw_db() noexcept
{
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




} // namespace revise
