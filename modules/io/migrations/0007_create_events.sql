-- Create 'events' table

CREATE TABLE IF NOT EXISTS events (
    id INTEGER PRIMARY KEY,
    type INTEGER NOT NULL,        -- type of event (for example, see core::EventType)
    created_at DATETIME NOT NULL,
    payload TEXT NOT NULL,        -- payload (for example, JSON description)
    sync_state INTEGER NOT NULL   -- synced with remote server?
)
