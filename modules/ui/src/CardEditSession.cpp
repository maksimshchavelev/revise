// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardEditSession.hpp" // for header
#include <QDebug>              // for debug messages

namespace ui {

CardEditSession::CardEditSession(core::ICardEditSession& card_edit_session, QObject* parent) :
    QObject(parent), m_card_edit_session(card_edit_session) {}


void CardEditSession::begin(QString front, QString back) {
    auto res = m_card_edit_session.begin(std::move(front), std::move(back));

    if (!res) {
        qWarning() << "Failed to begin card edit session, cause:" << res.error().description;
    }

    emit frontChanged();
    emit backChanged();
}


void CardEditSession::end() {
    auto res = m_card_edit_session.end();

    if (!res) {
        qWarning() << "Failed to end card edit session, cause:" << res.error().description;
    }
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
