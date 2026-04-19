// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for Launcher
#include <QtWebView>             // for QtWebView::initialize()

int main(int argc, char* argv[]) {
#ifndef Q_OS_ANDROID
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer");
#endif

    QtWebView::initialize();

    QGuiApplication app(argc, argv);

    revise::Launcher launcher(app);
    return launcher.run();
}
