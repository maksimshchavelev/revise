// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "PopupService.hpp" // for header

namespace ui {

PopupService::PopupService(core::IPopupService& popup_service, QObject* parent) :
    QObject(parent), m_popup_service(popup_service) {
    m_popup_service.connect<core::IPopupService::requested>([this](core::IPopupService::requested e) {
        QMetaObject::invokeMethod(
            this,
            [this, e]() {
                PopupRequest request(e.request);
                emit         requested(request);
            },
            Qt::QueuedConnection);
    });
}


void PopupService::response(const PopupResponse& response) {
    m_popup_service.response(response);
}

} // namespace ui