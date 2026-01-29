// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Notification manager

#include <NotificationManager/NotificationManager.hpp>
#include <QDebug>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
// #include <QtCore/private/qandroidextras_p.h>

static QJniObject get_android_context()
{
    // Qt 6.8 way to get android Context / Activity
    // Используем внутренний API для доступа к контексту
    QJniObject context = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;"
        );

    if (!context.isValid()) {
        // Альтернативный способ через контекст приложения
        context = QJniObject::callStaticObjectMethod(
            "org/qtproject/qt/android/QtNative",
            "context",
            "()Landroid/content/Context;"
            );
    }

    return context;
}

void NotificationManager::schedule_notification(const QString& text, const QDateTime& when, int id)
{
    QJniObject ctx = get_android_context();
    if (!ctx.isValid()) {
        qWarning() << "schedule_notification: no Android context";
        return;
    }

    QJniObject jText = QJniObject::fromString(text);
    jlong whenMs = static_cast<jlong>(when.toMSecsSinceEpoch());
    jint jId = static_cast<jint>(id);

    QJniObject::callStaticMethod<void>(
        "org/maksimshchavelev/revise/NotificationScheduler",
        "scheduleNotification",
        "(Landroid/content/Context;Ljava/lang/String;JI)V",
        ctx.object<jobject>(),
        jText.object<jstring>(),
        whenMs,
        jId);

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        qWarning() << "schedule_notification: Java exception";
    }
}

void NotificationManager::clear_all_scheduled_notifications()
{
    QJniObject ctx = get_android_context();
    if (!ctx.isValid()) {
        qWarning() << "clear_all_scheduled_notifications: no Android context";
        return;
    }

    QJniObject::callStaticMethod<void>(
        "org/maksimshchavelev/revise/NotificationScheduler",
        "clearAllScheduledNotifications",
        "(Landroid/content/Context;)V",
        ctx.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        qWarning() << "clear_all_scheduled_notifications: Java exception";
    }
}

#else

// stubs for other platforms
void NotificationManager::schedule_notification(const QString&, const QDateTime&, int) { qDebug() << "stub schedule_notification"; }
void NotificationManager::clear_all_scheduled_notifications() { qDebug() << "stub clear_all..."; }

#endif
