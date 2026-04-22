// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IStreakService.hpp> // for core::IStreakService
#include <core/IStreakStorage.hpp> // for core::IStreakStorage

namespace engine {

/**
 * @brief Implementation of the streak service with its own update logic.
 */
class StreakService final : public core::IStreakService {
  public:
    StreakService(core::IStreakStorage& storage);

    /// @copydoc core::IStreakService::get
    QFuture<Result<int>> get() const override;

    /// @copydoc core::IStreakService::set for details
    QFuture<Result<void>> set(int streak) override;

    /// @brief Resets the streak if it was last updated two or more days ago.
    /// @see `core::IStreakService` for details
    QFuture<Result<void>> reset_if_overdue() override;

    /// @copydoc core::IStreakService::overdue
    QFuture<Result<bool>> overdue() const override;

    /// @copydoc core::IStreakService::update
    QFuture<Result<void>> update() override;

    /// @copydoc core::IStreakService::updated_today
    QFuture<Result<bool>> updated_today() const override;

  private:
    core::IStreakStorage& m_storage;

    bool is_updated_today(const core::Streak& streak) const;

    bool is_overdue(const core::Streak& streak) const;
};

} // namespace engine