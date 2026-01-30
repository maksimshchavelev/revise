// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "platform/NotificationServiceFactory.hpp" // for NotificationServiceFactory header
#include <QtGlobal>                                // for Q_OS_ANDROID

#ifdef Q_OS_ANDROID
#include "AndroidNotificationService.hpp" // private include for AndroidNotificationService
#else                                     // end of Q_OS_ANDROID
#include "NotificationServiceStub.hpp"    // private include for NotificationServiceStub stub
#endif                                    // end of IFDEF

namespace platform {

std::unique_ptr<core::INotificationService> create_notification_service() {
#ifdef Q_OS_ANDROID
    return std::make_unique<internals::AndroidNotificationService>();
#else
    return std::make_unique<internals::NotificationServiceStub>(); // stub
#endif
}

} // namespace platform
