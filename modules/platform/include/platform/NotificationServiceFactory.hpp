// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/INotificationService.hpp> // for INotificationService
#include <memory>                        // for std::unique_ptr

namespace platform {

/**
 * @brief Creates a notification service, automatically selecting the implementation depending on the platform
 * @return `std::unique_ptr<core::INotificationService>`
 */
std::unique_ptr<core::INotificationService> create_notification_service();

} // namespace platform
