-- Initial migration to create certain tables

-- Creates an initial table of decks
CREATE TABLE IF NOT EXISTS decks (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    time_limit INTEGER DEFAULT 0,
    new_limit INTEGER DEFAULT 30,
    consolidate_limit INTEGER DEFAULT 30,
    incorrect_limit INTEGER DEFAULT 30
);

-- revise:statement
-- Creates an initial table of cards
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
);

-- revise:statement
-- The migration creates the 'streak' table if it does not exist
CREATE TABLE IF NOT EXISTS streak (
    id INTEGER PRIMARY KEY CHECK (id = 1),
    value INTEGER DEFAULT 0,
    last_updated DATETIME DEFAULT (DATETIME('now', 'localtime', '-1 day'))
);

-- revise:statement
-- The migration inserts a streak record into the streak table if it is empty
INSERT OR IGNORE INTO streak (id, value, last_updated)
VALUES (1, 0, DATETIME('now', 'localtime', '-1 day'));

-- revise:statement
-- Remove unused schema_versions table
DROP TABLE IF EXISTS schema_versions;
