// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/INotificationService.hpp>
#include <QString>
#include <QDateTime>

namespace platform::internals {

/**
* @brief Android implementation of INotificationService.
*
* Uses a Java helper (org.maksimshchavelev.revise.NotificationService) via JNI
* to show and schedule notifications.
*/
class AndroidNotificationService : public core::INotificationService {
public:
    /// See INotificationService
    void schedule_notification(const QString &text, const QDateTime &when) const override;

    /// See INotificationService
    void show_notification(const QString &text) const override;

    /// See INotificationService
    void clear_all_scheduled_notifications() const override;
};


} // namespace platform::internals
