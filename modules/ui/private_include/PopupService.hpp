// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Popup.hpp"              // for popup structures
#include <QObject>                // for QObject
#include <QQueue>                 // for QQueue
#include <core/IPopupService.hpp> // for IPopupService

namespace ui {

class PopupService final : public QObject {
    Q_OBJECT

  public:
    PopupService(core::IPopupService& popup_service, QObject* parent = nullptr);

    ///@ Response popup
    Q_INVOKABLE void response(const PopupResponse& response);

  signals:
    void requested(const PopupRequest& request);

  private:
    core::IPopupService& m_popup_service;
};

} // namespace ui