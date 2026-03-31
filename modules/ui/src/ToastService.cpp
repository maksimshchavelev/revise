// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ToastService.hpp" // for header

namespace ui {

ToastService::ToastService(core::IToastService& toast_service, QObject* parent) :
    QObject(parent), m_toast_service(toast_service) {
    m_toast_service.connect<core::IToastService::requested>([this](auto e) {
        ToastRequest toast{.header = std::move(e.toast.header),
                    .message = std::move(e.toast.message),
                    .type = static_cast<ToastRequest::ToastType>(e.toast.type)};
        emit  requested(std::move(toast));
    });
}

void ToastService::closed() {
    m_toast_service.close_last();
}

} // namespace ui