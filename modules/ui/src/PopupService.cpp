// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "PopupService.hpp" // for PopupService header

namespace ui {

PopupService::PopupService(QObject* parent) : QObject(parent) {}

void PopupService::request(const core::PopupRequest&                       request,
                           std::function<void(const core::PopupResponse&)> callback) {
    ui::PopupRequest req;

    req.type = request.type;
    req.context = request.context;
    req.message = request.message;

    for (const auto& button : request.buttons) {
        PopupButton btn;

        btn.clickable = button.clickable;
        btn.id = button.id;
        btn.text = button.text;

        req.buttons.append(QVariant::fromValue(std::move(btn)));
    }

    emit requested(req);
    m_requests.enqueue({.request = std::move(req), .callback = callback});
}

void PopupService::response(const PopupResponse& response) {
    // Send a signal that the response has been received, call the callback, and display the next popup, if there is
    // one.

    Pending req = m_requests.dequeue();
    req.callback(core::PopupResponse(response));

    emit responsed(response);

    // Show next
    if (!m_requests.isEmpty()) {
        emit requested(m_requests.head().request);
    }
}

} // namespace ui
