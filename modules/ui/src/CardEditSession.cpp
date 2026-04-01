// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardEditSession.hpp" // for header
#include <QDebug>              // for debug messages
#include <utils/Html.hpp>      // for html utils

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
    QString front = m_card_edit_session.front();

    if (utils::Html::new_lines(front) < 3) {
        front = utils::Html::center_horizontal(front);
    }

    return utils::Html::center_vertical(front);
}


void CardEditSession::set_front(QString front) {
    emit frontChanged();
    m_card_edit_session.set_front(std::move(front));
}


QString CardEditSession::back() {
    QString back = m_card_edit_session.back();

    if (utils::Html::new_lines(back) < 3) {
        back = utils::Html::center_horizontal(back);
    }

    return utils::Html::center_vertical(back);
}


void CardEditSession::set_back(QString back) {
    emit backChanged();
    m_card_edit_session.set_back(std::move(back));
}



} // namespace ui
