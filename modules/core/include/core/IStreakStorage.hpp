// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Streak.hpp" // for Streak
#include <expected>   // for std::expected

namespace core {

/**
 * @brief An interface that abstracts working with streak storage. Through this class, you can get the current streak
 * and save it.
 */
class IStreakStorage {
  public:
    virtual ~IStreakStorage() = default;

    /**
     * @brief Saves the current streak
     * @param streak The `core::Streak` structure describing a streak
     * @return `std::expected <void, QString>` with `void` if success, otherwise with error description
     */
    virtual std::expected<void, QString> save(const Streak streak) = 0;

    /**
     * @brief Reads the current streak
     * @return `std::expected <void, QString>` with `core::Streak` structure describing a streak if success, otherwise
     * with error description
     */
    virtual std::expected<void, Streak> read() = 0;
};

} // namespace core