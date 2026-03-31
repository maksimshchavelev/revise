// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ToastService.hpp" // for header

namespace engine {

void ToastService::request(core::Toast toast) {
    m_requests.enqueue(std::move(toast));

    if (m_requests.size() == 1) {
        dispatch(requested{m_requests.head()});
    }
}

void ToastService::close_last() {
    if (m_requests.empty()) {
        return;
    }

    m_requests.dequeue();

    if (!m_requests.empty()) {
        dispatch(requested{m_requests.head()});
    }
}

} // namespace engine
