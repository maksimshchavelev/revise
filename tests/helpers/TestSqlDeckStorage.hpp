// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "TestDatabase.hpp"                // for TestDatabase
#include <io/DatabaseExecutionContext.hpp> // for io::DatabaseExecutionContext
#include <io/SqlDeckStorage.hpp>           // for io::SqlDeckStorage

namespace helpers {

/// @brief RAII struct with `io::SqlDeckStorage` inside
struct TestSqlDeckStorage {
  private:
    TestDatabase                 database{create_database()}; ///< Created database
    io::DatabaseExecutionContext context;                     ///< Database execution context

  public:
    io::SqlDeckStorage storage; ///< Ready to use SqlDeckStorage

    TestSqlDeckStorage() : storage{database.database, context} {}
};

/// @brief Create `TestSqlDeckStorage` with `io::SqlDeckStorage` inside
inline TestSqlDeckStorage create_storage() {
    return TestSqlDeckStorage{};
}

} // namespace helpers
