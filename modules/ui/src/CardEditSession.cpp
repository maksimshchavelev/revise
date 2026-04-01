// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardEditSession.hpp" // for header

namespace ui {

CardEditSession::CardEditSession(core::ICardEditSession& card_edit_session, QObject* parent) :
    QObject(parent), m_card_edit_session(card_edit_session) {}


void CardEditSession::begin(QString front, QString back) {
    m_card_edit_session.begin(std::move(front), std::move(back));
}


void CardEditSession::end() {
    m_card_edit_session.end();
}


QString CardEditSession::front() {
    return m_card_edit_session.front();
}


void CardEditSession::set_front(QString front) {
    emit frontChanged();
    m_card_edit_session.set_front(std::move(front));
}


QString CardEditSession::back() {
    return m_card_edit_session.back();
}


void CardEditSession::set_back(QString back) {
    emit backChanged();
    m_card_edit_session.set_back(std::move(back));
}



} // namespace ui
