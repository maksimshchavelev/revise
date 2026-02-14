// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/UI.hpp" // for UI header
#include "PopupService.hpp"
#include "StreakService.hpp" // for StreakService
#include <QQmlContext>       // for QQmlContext

namespace ui {

void UI::init_qml() {
    // engine.rootContext()->setContextProperty("popupService", dynamic_cast<PopupService*>(&popup_service));

    qRegisterMetaType<PopupRequest>("PopupRequest");
    qRegisterMetaType<PopupResponse>("PopupResponse");
    qRegisterMetaType<PopupAction>("PopupAction");
    qRegisterMetaType<PopupButton>("PopupButton");
}


void UI::init_engine(QGuiApplication& app) {
    m_engine.addImportPath("qrc:/");

    QObject::connect(
        &m_engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    m_engine.loadFromModule("Revise", "App");
}


void UI::bind_streak_service(core::IStreakService& streak_service) {
    auto* service = new StreakService(streak_service, &m_engine);

    m_engine.rootContext()->setContextProperty("streakService", service);
}

} // namespace ui