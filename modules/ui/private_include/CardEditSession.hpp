// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/ICardEditSession.hpp>          // for core::ICardEditSession

namespace ui {

/// @brif CardEditSession wrapper
class CardEditSession : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("CardEditSession is provided by C++")

  public:
    Q_PROPERTY(QString front READ front WRITE set_front NOTIFY frontChanged FINAL)
    Q_PROPERTY(QString back READ back WRITE set_back NOTIFY backChanged FINAL)

    explicit CardEditSession(core::ICardEditSession& card_edit_session, QObject* parent = nullptr);

    /// @brief Begin editing
    Q_INVOKABLE void begin(QString front, QString back);

    /// @brief Finish editing
    Q_INVOKABLE void end();

    /// @brief Get front of card
    QString front();

    /// @brief Set front of card
    void set_front(QString front);

    /// @brief Get back of card
    QString back();

    /// @brief Get back of card
    void set_back(QString back);

  signals:
    void frontChanged();
    void backChanged();

  private:
    core::ICardEditSession& m_card_edit_session;
};

} // namespace ui
