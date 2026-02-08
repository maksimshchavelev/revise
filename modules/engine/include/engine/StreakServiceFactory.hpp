// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IStreakService.hpp> // for core::IStreakService
#include <memory>                  // for std::unique_ptr

namespace engine {

/**
 * @brief The function creates a specific implementation of IStreakService and returns a pointer to it.
 * @param storage Streak storage
 * @return `std::unique_ptr<core::IStreakService>`
 */
std::unique_ptr<core::IStreakService> create_streak_service(std::shared_ptr<core::IStreakStorage> storage);

} // namespace engine
