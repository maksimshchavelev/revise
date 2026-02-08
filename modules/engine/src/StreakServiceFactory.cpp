// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "engine/StreakServiceFactory.hpp" // for header
#include "StreakService.hpp"               // for StreakService

namespace engine {

std::unique_ptr<core::IStreakService> create_streak_service(std::shared_ptr<core::IStreakStorage> storage) {
    return std::make_unique<StreakService>(storage);
}

} // namespace engine
