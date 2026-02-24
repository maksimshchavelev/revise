// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "PopupService.hpp" // for header

namespace engine {

void PopupService::request(const core::PopupRequest&                       request,
                           std::function<void(const core::PopupResponse&)> callback) {
    m_requests.enqueue(Request{.request = std::move(request), .callback = std::move(callback)});

    // If there is a popup in the queue that has just been requested
    if (m_requests.size() == 1) {
        dispatch(requested{m_requests.head().request});
    }
}

void PopupService::response(const core::PopupResponse& response) {
    if (m_requests.empty()) {
        return;
    }

    auto req = m_requests.dequeue();
    req.callback(response);

    if (!m_requests.empty()) {
        dispatch(requested{m_requests.head().request});
    }
}

} // namespace engine
