// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for header
#include <QtWebView>             // for QtWebView::initialize()

#include <engine/StreakServiceFactory.hpp> // for engine::create_streak_service


namespace revise {

Launcher::Launcher(QGuiApplication& app) : m_app(app) {}


int Launcher::run() {
#ifndef Q_OS_ANDROID
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-software-rasterizer");
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QtWebView::initialize();

    init();

    if (auto res = m_streak_storage->migrate(); !res) {
        qWarning() << "Failed to apply streak storage migrations:" << res.error();
    }

    m_ui.init_engine(m_app);
    m_ui.bind_streak_service(*m_streak_service);

    return m_app.exec();
}


void Launcher::init() {
    if (m_streak_storage = std::make_shared<io::SqlStreakStorage>(m_db, m_db_context); m_streak_storage) {
        qDebug() << "Streak storage created";
    }

    if (m_streak_service = engine::create_streak_service(m_streak_storage); m_streak_service) {
        qDebug() << "Streak service created";
    }
}


} // namespace revise