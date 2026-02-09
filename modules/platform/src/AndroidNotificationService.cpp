// modules/platform/src/AndroidNotificationService.cpp

#include "AndroidNotificationService.hpp"
#include <QDebug>
#include <QString>
#include <QDateTime>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QJniEnvironment>
#endif

namespace platform::internals {

#ifdef Q_OS_ANDROID
static QJniObject get_activity()
{
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;");
    return activity;
}
#endif

void AndroidNotificationService::show_notification(const QString &text) const
{
    #ifdef Q_OS_ANDROID
    const QJniObject jtext = QJniObject::fromString(text);
    QJniObject activity = get_activity();
    if (!activity.isValid()) {
        qWarning() << "show_notification: no Activity";
        return;
    }

    QJniObject service("org/maksimshchavelev/revise/NotificationService",
                       "(Landroid/content/Context;)V",
                       activity.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); return; }
    if (!service.isValid()) { qWarning() << "Java NotificationService invalid"; return; }

    service.callMethod<void>("showNotification", "(Ljava/lang/String;)V", jtext.object<jstring>());
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
    #endif
}

void AndroidNotificationService::schedule_notification(const QString &text, const QDateTime &when) const
{
    #ifdef Q_OS_ANDROID
    const qint64 msecs = when.toMSecsSinceEpoch();
    const jlong epochMillis = static_cast<jlong>(msecs);
    const QJniObject jtext = QJniObject::fromString(text);

    // Direct call to Java scheduleNotification(String, long)
    QJniObject activity = get_activity();
    if (!activity.isValid()) { qWarning() << "schedule_notification: no Activity"; return; }

    QJniObject service("org/maksimshchavelev/revise/NotificationService",
                       "(Landroid/content/Context;)V",
                       activity.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); return; }
    if (!service.isValid()) { qWarning() << "Java NotificationService invalid"; return; }

    service.callMethod<void>("scheduleNotification", "(Ljava/lang/String;J)V", jtext.object<jstring>(), epochMillis);
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
    #endif
}

void AndroidNotificationService::clear_all_scheduled_notifications() const
{
    #ifdef Q_OS_ANDROID
    QJniObject activity = get_activity();
    if (!activity.isValid()) { qWarning() << "clear_all_scheduled_notifications: no Activity"; return; }

    QJniObject service("org/maksimshchavelev/revise/NotificationService",
                       "(Landroid/content/Context;)V",
                       activity.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); return; }
    if (!service.isValid()) { qWarning() << "Java NotificationService invalid"; return; }

    service.callMethod<void>("clearAllScheduledNotifications", "()V");
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
    #endif
}

} // namespace platform::internals
