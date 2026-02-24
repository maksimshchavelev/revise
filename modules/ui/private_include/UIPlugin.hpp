// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Popup.hpp" // for Popup types
#include <QString>   // for QString
#include <QtQml>     // for QQmlExtensionPlugin

namespace ui {

class UIPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

  public:
    void registerTypes(const char* uri) override {
        qmlRegisterUncreatableMetaObject(PopupType::staticMetaObject, "ReviseTypes", 1, 0, "PopupType", "enum only");
        qmlRegisterType<PopupButton>("Revise", 1, 0, "PopupButton");
        qmlRegisterType<PopupRequest>("Revise", 1, 0, "PopupRequest");
    }
};

} // namespace ui
