// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IPopupService.hpp> // for IPopupService
#include <memory>                 // for std::unique_ptr

namespace ui {

/**
 * @brief Creates a service for displaying popup messages
 * @return `std::unique_ptr<core::IPopupService>`
 * @see `core::IPopupService`
 */
std::unique_ptr<core::IPopupService> create_popup_service();

} // namespace ui