// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Notification manager

#pragma once

#include <QString>
#include <QDateTime>

/// NotificationManager: cross-platform wrapper for scheduling notifications on Android.
/// Methods are static and use snake_case naming as requested.
class NotificationManager {
public:
    /**
     * @brief Schedule a single notification.
     * @param text Notification text.
     * @param when Date/time (local) when notification should fire.
     * @param id Unique integer id for the notification (used to cancel).
     */
    static void schedule_notification(const QString& text, const QDateTime& when, int id);

    /**
     * @brief Cancel all scheduled notifications created by the app.
     */
    static void clear_all_scheduled_notifications();
};
