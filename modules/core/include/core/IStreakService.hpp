// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "IStreakStorage.hpp" // for IStreakStorage
#include "Notifiable.hpp"     // for Notifiable
#include <expected>           // for std::expected

namespace core {

/**
 * @brief An interface that abstracts working with streak storage. Through this class, you can get the current streak
 * and save it.
 */
class IStreakService : public Notifiable {
  public:
    EVENT(updated, int value /* new value */) /// streak updated
    EVENT(reset)                              /// streak reset

    IStreakService(std::shared_ptr<IStreakStorage> storage) : m_storage(storage) {}

    virtual ~IStreakService() = default;

    /**
     * @brief Get the current streak
     * @return `std::expected<int, QString>` with streak value if success, otherwise error description
     */
    virtual std::expected<int, QString> get() = 0;

    /**
     * @brief Set the streak
     * @param streak Streak
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> set(int streak) = 0;

    /**
     * @brief Resets the streak if it is overdue. How exactly the overdue criterion is determined is determined by the
     * implementation.
     * @return `std::expected<int, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> reset_if_overdue() = 0;

    /**
     * @brief Check if your streak has expired
     * @return `true` if yes
     */
    virtual std::expected<bool, QString> overdue() const = 0;

    /**
     * @brief Updates the streak. If the strike has already been updated today, it does nothing.
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> update() = 0;

    /**
     * @brief Check if the streak has been updated today
     * @return `std::expected<bool, QString>` with `true` if yes (or `false` if no), otherwise error description
     */
    virtual std::expected<bool, QString> updated_today() const = 0;

  protected:
    std::shared_ptr<IStreakStorage> m_storage;
};

} // namespace core