// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <../../core/include/core/service/IPermissionService.hpp>

namespace platform::internals {

class AndroidPermissionService : public core::IPermissionService {
  public:
    /// See IPermissionService docs
    bool check(core::Permission permission) const noexcept override;

    /// See IPermissionService docs
    void request(core::Permission permission) const noexcept override;
};

} // namespace platform::internals
