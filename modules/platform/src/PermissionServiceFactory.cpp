// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "platform/PermissionServiceFactory.hpp" // for PermissionServiceFactory header
#include <QtGlobal>                              // for Q_OS_ANDROID

#ifdef Q_OS_ANDROID
#include "AndroidPermissionService.hpp" // private include for AndroidPermissionService
#else                                   // end of Q_OS_ANDROID
#include "PermissionServiceStub.hpp"    // private include for PermissionService stub
#endif                                  // end of IFDEF

namespace platform {

std::unique_ptr<core::IPermissionService> create_permission_service() {
#ifdef Q_OS_ANDROID
    return std::make_unique<internals::AndroidPermissionService>();
#else
    return std::make_unique<internals::PermissionServiceStub>(); // stub
#endif
}

} // namespace platform
