// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IToastService.hpp> // for core::IToastService
#include <memory>                 // for std::unique_ptr

namespace engine {

/**
 * @brief The function creates a specific implementation of core::IToastService
 */
std::unique_ptr<core::IToastService> create_toast_service();

} // namespace engine
