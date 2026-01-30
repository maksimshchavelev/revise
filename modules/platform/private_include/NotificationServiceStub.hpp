// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QDateTime>
#include <QString>
#include <core/INotificationService.hpp>

namespace platform::internals {

/**
 * @brief NotificationServiceStub stub
 */
class NotificationServiceStub : public core::INotificationService {
  public:
    /// See INotificationService
    void schedule_notification(const QString& text, const QDateTime& when) const override;

    /// See INotificationService
    void show_notification(const QString& text) const override;

    /// See INotificationService
    void clear_all_scheduled_notifications() const override;
};

} // namespace platform::internals
