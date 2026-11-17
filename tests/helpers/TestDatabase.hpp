// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QTemporaryDir>   // for QTemporaryDir
#include <io/Database.hpp> // for io::Database

namespace helpers {

/// @brief RAII struct with `io::Database` inside and automatically database file cleanup
struct TestDatabase {
  private:
    QTemporaryDir database_directory; ///< Directory where database will be created

  public:
    io::Database database; ///< Created database

    /// Create `TestDatabase` instance
    explicit TestDatabase(QTemporaryDir dir) :
        database_directory{std::move(dir)}, database{"revise_test", database_directory.path() + "/revise_test.db"} {
        if (const auto res = database.migrate(); !res) {
            throw std::runtime_error(QString("TestDatabase(): failed to apply migrations: %1").arg(res.error()).toStdString());
        }
    }

    friend TestDatabase create_database();
};

/// @brief Create SQL database with random unique name inside specific directory near test executable
inline TestDatabase create_database() {
    QTemporaryDir database_directory{};

    if (!database_directory.isValid()) {
        throw std::runtime_error("Failed to create temporary directory for test database");
    }

    return TestDatabase{std::move(database_directory)};
}

} // namespace helpers
