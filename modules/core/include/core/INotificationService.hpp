// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>
#include <QDateTime>

namespace core {

/**
 * @brief The NotificationManager class
 */
class INotificationService {
public:
    virtual ~INotificationService() = default;

    /**
     * @brief Schedule a single notification.
     * @param text Notification text.
     * @param when Date/time (local) when notification should fire.
     */
    virtual void schedule_notification(const QString& text, const QDateTime& when) const = 0;

    /**
     * @brief Show single notification now
     * @param text Text of the notification
     */
    virtual void show_notification(const QString& text) const = 0;

    /**
     * @brief Cancel all scheduled notifications created by the app.
     */
    virtual void clear_all_scheduled_notifications() const = 0;
};

}
