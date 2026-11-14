// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for Launcher
#include <QtWebView>             // for QtWebView::initialize()

#ifdef Q_OS_ANDROID
#include <jni.h> // for JNI
extern "C" JNIEXPORT jboolean JNICALL Java_org_qtproject_qt_android_QtNativeAccessibility_accessibilitySupported(JNIEnv*,
                                                                                                                 jobject) {
    return JNI_FALSE;
}
#endif

int main(int argc, char* argv[]) {
#ifdef Q_OS_ANDROID
    qputenv("QSG_NO_VSYNC", "1");
#endif

#ifndef Q_OS_ANDROID
    qputenv("QT_WEBVIEW_PLUGIN", "webengine");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer");
#endif

    QtWebView::initialize();

    QGuiApplication app(argc, argv);
    QGuiApplication::setWindowIcon(QIcon{":/res/img/icon.png"});

    revise::Launcher launcher(app);
    return launcher.run();
}
