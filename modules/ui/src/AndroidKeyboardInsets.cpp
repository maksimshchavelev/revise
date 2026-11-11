// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "AndroidKeyboardInsets.hpp" // for AndroidKeyboardInsets
#include <QJniObject>                // for QJniObject
#include <QMetaObject>               // for QMetaObject
#include <QScreen>                   // for QScreen
#include <QVariant>                  // for QVariant
#include <QtGlobal>                  // for Q_OS_ANDROID, qFuzzyCompare

#ifdef Q_OS_ANDROID
#include <jni.h> // for JNI types and JNIEXPORT
#endif

namespace ui {
AndroidKeyboardInsets::AndroidKeyboardInsets(QObject* parent) : QObject(parent) {}

void AndroidKeyboardInsets::install() {
#ifdef Q_OS_ANDROID
    const jlong nativeHandle = reinterpret_cast<quintptr>(this);

    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([nativeHandle]() -> QVariant {
        if (!QNativeInterface::QAndroidApplication::isActivityContext()) {
            qWarning() << "AndroidKeyboardInsets: Activity context is unavailable";
            return QVariant::fromValue(false);
        }

        const QJniObject activity = QNativeInterface::QAndroidApplication::context();

        if (!activity.isValid()) {
            qWarning() << "AndroidKeyboardInsets: Invalid Android Activity";
            return QVariant::fromValue(false);
        }

        const jboolean installed = QJniObject::callStaticMethod<jboolean>("org/maksimshchavelev/revise/KeyboardInsets",
                                                                          "install",
                                                                          "(Landroid/app/Activity;J)Z",
                                                                          activity.object<jobject>(),
                                                                          nativeHandle);

        if (installed != JNI_TRUE) {
            qWarning() << "AndroidKeyboardInsets: Failed to install listener";
        }

        return QVariant::fromValue(installed == JNI_TRUE);
    });
#endif
}

qreal AndroidKeyboardInsets::imeHeight() const noexcept {
    return m_imeHeight;
}

bool AndroidKeyboardInsets::imeVisible() const noexcept {
    return m_imeVisible;
}

void AndroidKeyboardInsets::dispatchImeInsets(int bottomPx, bool visible) {
    QMetaObject::invokeMethod(this, [this, bottomPx, visible] { updateInsets(bottomPx, visible); }, Qt::QueuedConnection);
}

void AndroidKeyboardInsets::updateInsets(int bottomPx, bool visible) {
    qreal devicePixelRatio = 1.0;

    if (const QScreen* screen = QGuiApplication::primaryScreen()) {
        devicePixelRatio = screen->devicePixelRatio();
    }

    if (devicePixelRatio <= 0.0) {
        devicePixelRatio = 1.0;
    }

    const qreal height = qMax(0.0, static_cast<qreal>(bottomPx) / devicePixelRatio);

    if (m_imeVisible == visible && qFuzzyCompare(m_imeHeight + 1.0, height + 1.0)) {
        return;
    }

    m_imeHeight = height;
    m_imeVisible = visible;

    qInfo() << "IME:"
            << "visible =" << visible << "height =" << height;

    emit imeInsetsChanged();
}

#ifdef Q_OS_ANDROID

extern "C" JNIEXPORT void JNICALL Java_org_maksimshchavelev_revise_KeyboardInsets_nativeOnImeInsetsChanged(
    JNIEnv*, jclass, jlong nativeHandle, jint bottomPx, jboolean visible) {
    auto* instance = reinterpret_cast<AndroidKeyboardInsets*>(static_cast<quintptr>(nativeHandle));

    if (!instance) {
        return;
    }

    instance->dispatchImeInsets(bottomPx, visible == JNI_TRUE);
}

#endif
} // namespace ui
