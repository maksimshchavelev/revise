-- Creates an initial table of decks

CREATE TABLE IF NOT EXISTS decks (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    time_limit INTEGER DEFAULT 0,
    new_limit INTEGER DEFAULT 30,
    consolidate_limit INTEGER DEFAULT 30,
    incorrect_limit INTEGER DEFAULT 30
)