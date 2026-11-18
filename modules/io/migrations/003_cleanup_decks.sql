-- Recreate decks table without global_id, source and version columns

CREATE TABLE decks_new (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    time_limit INTEGER DEFAULT 0,
    new_limit INTEGER DEFAULT 30,
    consolidate_limit INTEGER DEFAULT 30,
    incorrect_limit INTEGER DEFAULT 30
);

-- revise:statement
INSERT INTO decks_new (
    id,
    name,
    description,
    time_limit,
    new_limit,
    consolidate_limit,
    incorrect_limit
)
SELECT
    id,
    name,
    description,
    time_limit,
    new_limit,
    consolidate_limit,
    incorrect_limit
FROM decks;

-- revise:statement
DROP TABLE decks;

-- revise:statement
ALTER TABLE decks_new RENAME TO decks;
