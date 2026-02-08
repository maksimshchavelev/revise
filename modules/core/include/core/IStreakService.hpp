// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "IStreakStorage.hpp" // for IStreakStorage
#include <expected>           // for std::expected

namespace core {

/**
 * @brief An interface that abstracts working with streak storage. Through this class, you can get the current streak
 * and save it.
 */
class IStreakService {
  public:
    IStreakService(std::shared_ptr<IStreakStorage> storage);

    virtual ~IStreakService() = default;

    /**
     * @brief Get the current streak
     * @return `std::expected<Streak, QString>` with `core::Streak` if success, otherwise error description
     */
    virtual std::expected<Streak, QString> get() = 0;

    /**
     * @brief Set the streak
     * @param streak Streak
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> set(const Streak& streak) = 0;

    /**
     * @brief Resets the streak if it is overdue. How exactly the overdue criterion is determined is determined by the
     * implementation.
     * @return `std::expected<int, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> reset_if_overdue() = 0;

  private:
    std::shared_ptr<IStreakStorage> m_storage;
};

} // namespace core