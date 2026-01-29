// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IPermissionService.hpp> // for IPermissionService
#include <memory>                      // for std::unique_ptr

namespace platform {

/**
 * @brief Creates a permission request service, automatically selecting the implementation depending on the platform
 * @return `std::unique_ptr<core::IPermissionService>`
 */
std::unique_ptr<core::IPermissionService> create_permission_service();

} // namespace platform
