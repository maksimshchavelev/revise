// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                   // for QObject
#include <core/ICardEditSession.hpp> // for core::ICardEditSession

namespace ui {

/// @brif CardEditSession wrapper
class CardEditSession : public QObject {
  public:
    Q_PROPERTY(QString front READ front WRITE set_front NOTIFY frontChanged FINAL)
    Q_PROPERTY(QString back READ back WRITE set_back NOTIFY backChanged FINAL)

    CardEditSession(core::ICardEditSession& card_edit_session, QObject* parent = nullptr);

    Q_INVOKABLE void begin(QString front, QString back);

    Q_INVOKABLE void end();

    QString front();

    void set_front(QString front);

    QString back();

    void set_back(QString back);

  signals:
    void frontChanged();
    void backChanged();

  private:
    core::ICardEditSession& m_card_edit_session;
};

} // namespace ui
