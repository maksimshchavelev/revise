// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "PermissionServiceStub.hpp" // private include for PermissionServiceStub header

namespace platform::internals {

bool PermissionServiceStub::check(core::Permission permission) const noexcept {
    return true; // do nothing
}

void PermissionServiceStub::request(core::Permission permission) const noexcept {
    // do nothing
}

} // namespace platform::internals
