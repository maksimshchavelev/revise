// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardEditSession.hpp" // for header

namespace engine {

std::expected<void, core::CardEditSessionError> CardEditSession::begin(QString front, QString back) {
    m_front = std::move(front);
    m_back = std::move(back);

    return {};
}

std::expected<void, core::CardEditSessionError> CardEditSession::end() {
    dispatch(editing_finished{m_front, m_back});

    return {};
}

void CardEditSession::set_front(QString front) {
    m_front = std::move(front);
}

QString CardEditSession::front() const {
    return m_front;
}

void CardEditSession::set_back(QString back) {
    m_back = std::move(back);
}

QString CardEditSession::back() const {
    return m_back;
}

} // namespace engine
