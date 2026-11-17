-- The migration creates the 'streak' table if it does not exist

CREATE TABLE IF NOT EXISTS streak (
    id INTEGER PRIMARY KEY CHECK (id = 1),
    value INTEGER DEFAULT 0,
    last_updated DATETIME DEFAULT (DATETIME('now', 'localtime', '-1 day'))
);