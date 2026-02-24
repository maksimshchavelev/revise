// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IPopupService.hpp> // for core::IPopupService
#include <memory>                 // for std::unique_ptr

namespace engine {

/**
 * @brief The function creates a specific implementation of core::IPopupService
 */
std::unique_ptr<core::IPopupService> create_popup_service();

} // namespace engine
