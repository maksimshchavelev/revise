// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "AndroidPermissionService.hpp" // private include for AndroidPermissionService header
#include <QCoreApplication>
#include <QJniObject> // for QJniObject
#include <QString>

namespace platform::internals {

// helper
static QString get_permission(core::Permission permission) {
    switch (permission) {
    case core::Permission::POST_NOTIFICATIONS:
        return "android.permission.POST_NOTIFICATIONS";

    case core::Permission::INTERNET:
        return "android.permission.INTERNET";

    case core::Permission::READ_EXTERNAL_STORAGE:
        return "android.permission.READ_EXTERNAL_STORAGE";

    case core::Permission::WRITE_EXTERNAL_STORAGE:
        return "android.permission.WRITE_EXTERNAL_STORAGE";

    case core::Permission::SCHEDULE_EXACT_ALARM:
        return "android.permission.SCHEDULE_EXACT_ALARM";

    default:
        return QString();
    }
}

bool AndroidPermissionService::check(core::Permission permission) const noexcept {
    const QString perm = get_permission(permission);

    if (perm.isEmpty()) {
        return false; // fallback
    }

    // Obtain Activity via QtNative.activity()
    QJniObject activity =
        QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");

    if (!activity.isValid()) {
        qWarning() << "AndroidPermissionService::check: QtNative.activity() returned invalid object.";
        return false;
    }

    // Create Java PermissionService(Activity) object
    QJniObject service(
        "org/maksimshchavelev/revise/PermissionService", "(Landroid/app/Activity;)V", activity.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        qWarning() << "AndroidPermissionService::check: exception while creating PermissionService";
        return false;
    }

    if (!service.isValid()) {
        qWarning() << "AndroidPermissionService::check: PermissionService instance is invalid";
        return false;
    }

    // Call boolean hasPermission(String)
    jboolean granted = service.callMethod<jboolean>(
        "hasPermission", "(Ljava/lang/String;)Z", QJniObject::fromString(perm).object<jstring>());

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        qWarning() << "AndroidPermissionService::check: exception during hasPermission call";
        return false;
    }

    return granted == JNI_TRUE;
}

void AndroidPermissionService::request(core::Permission permission) const noexcept {
    const QString perm = get_permission(permission);

    if (perm.isEmpty()) {
        return;
    }

    QJniObject activity =
        QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");

    if (!activity.isValid()) {
        qWarning() << "No activity from QtNative.activity()";
        return;
    }

    // create instance
    QJniObject service(
        "org/maksimshchavelev/revise/PermissionService", "(Landroid/app/Activity;)V", activity.object<jobject>());

    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    if (!service.isValid()) {
        qWarning() << "PermissionService not created";
        return;
    }

    service.callMethod<void>(
        "requestPermission", "(Ljava/lang/String;)V", QJniObject::fromString(perm).object<jstring>());

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

} // namespace platform::internals
