-- The migration inserts a streak record into the streak table if it is empty

INSERT OR IGNORE INTO streak (id, value, last_updated)
VALUES (1, 0, DATETIME('now', 'localtime', '-1 day'));