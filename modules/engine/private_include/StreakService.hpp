// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IStreakService.hpp> // for core::IStreakService

namespace engine {

/**
 * @brief Implementation of the streak service with its own update logic.
 */
class StreakService final : public core::IStreakService {
  public:
    StreakService(std::shared_ptr<core::IStreakStorage> storage);

    /// @see `core::IStreakService` for details
    std::expected<core::Streak, QString> get() override;

    /// @see `core::IStreakService` for details
    std::expected<void, QString> set(const core::Streak& streak) override;

    /**
     * @brief Resets the streak if it was last updated two or more days ago.
     * @see `core::IStreakService` for details
     */
    std::expected<void, QString> reset_if_overdue() override;
};

} // namespace engine