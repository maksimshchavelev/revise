// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/UI.hpp"   // for UI header
#include "PopupService.hpp"
#include <QQmlContext> // for QQmlContext

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

} // namespace ui