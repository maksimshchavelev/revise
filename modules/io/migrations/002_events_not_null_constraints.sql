-- The migration deletes the old `events` table, removes the default value of 0 for
-- the `type` and `sync_state` columns, and adds a `NOT NULL` constraint to them.

DROP TABLE IF EXISTS events;

-- revise:statement
CREATE TABLE IF NOT EXISTS events (
    id INTEGER PRIMARY KEY,
    type INTEGER NOT NULL,        -- type of event (for example, see core::EventType)
    created_at DATETIME NOT NULL,
    payload TEXT NOT NULL,        -- payload (for example, JSON description)
    sync_state INTEGER NOT NULL   -- synced with remote server?
);
