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
)