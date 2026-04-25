// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

namespace io {

constexpr int SQLITE_PERM = 3;
constexpr int SQLITE_BUSY = 5;
constexpr int SQLITE_LOCKED = 6;
constexpr int SQLITE_READONLY = 8;
constexpr int SQLITE_IOERR = 10;
constexpr int SQLITE_FULL = 13;
constexpr int SQLITE_CANTOPEN = 14;
constexpr int SQLITE_MISMATCH = 20;
constexpr int SQLITE_RANGE = 25;

constexpr int SQLITE_CONSTRAINT_CHECK = 275;
constexpr int SQLITE_BUSY_TIMEOUT = 773;
constexpr int SQLITE_CONSTRAINT_FOREIGNKEY = 787;
constexpr int SQLITE_CONSTRAINT_NOTNULL = 1299;
constexpr int SQLITE_CONSTRAINT_PRIMARYKEY = 1555;
constexpr int SQLITE_CONSTRAINT_UNIQUE = 2067;

} // namespace io
