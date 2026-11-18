// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <TestDatabase.hpp>                // for TestDatabase helper
#include <gtest/gtest.h>                   // for gtest
#include <io/DatabaseExecutionContext.hpp> // for io::DatabaseExecutionContext
#include <io/SqlStreakStorage.hpp>         // for io::SqlStreakStorage
#include <QDateTime>                       // for QDateTime

using namespace io;

// ------------------------ State of the newly created streak storage ------------------------
TEST(sql_streak_storage, new_storage_contains_zero_streak) {
    auto database = helpers::create_database();
    DatabaseExecutionContext context{};

    const auto storage = SqlStreakStorage{database.database, context};
    const auto streak = storage.read();

    ASSERT_TRUE(streak);
    EXPECT_LE(streak->value, 0);
    EXPECT_LE(streak->updated_at, QDateTime::currentDateTime().addDays(-1));
}


// ------------------------ Saving and reading the streak ------------------------
TEST(sql_streak_storage, save_and_read_streak) {
    auto database = helpers::create_database();
    DatabaseExecutionContext context{};

    auto storage = SqlStreakStorage{database.database, context};

    const QDateTime expected_date = QDateTime::currentDateTime().addDays(-5);
    const core::Streak expected_streak{42, expected_date};

    ASSERT_TRUE(storage.save(expected_streak));

    const auto actual_streak = storage.read();

    ASSERT_TRUE(actual_streak);
    EXPECT_EQ(actual_streak->value, expected_streak.value);
    EXPECT_EQ(actual_streak->updated_at, expected_streak.updated_at);
}


// ------------------------ Overwriting the existing streak ------------------------
TEST(sql_streak_storage, save_overwrites_existing_streak) {
    auto database = helpers::create_database();
    DatabaseExecutionContext context{};

    auto storage = SqlStreakStorage{database.database, context};

    const core::Streak first_streak{10, QDateTime::currentDateTime().addDays(-2)};
    const core::Streak second_streak{20, QDateTime::currentDateTime()};

    ASSERT_TRUE(storage.save(first_streak));
    ASSERT_TRUE(storage.save(second_streak));

    const auto actual_streak = storage.read();

    ASSERT_TRUE(actual_streak);
    EXPECT_EQ(actual_streak->value, second_streak.value);
    EXPECT_EQ(actual_streak->updated_at, second_streak.updated_at);
}

