// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for Q_GADGET
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/Card.hpp>                      // for core::Card

namespace ui {

/**
 * @brief Wrapper over core::Card, visible from QML
 */
struct Card final : core::Card {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(int id MEMBER id FINAL)
    Q_PROPERTY(int global_id MEMBER global_id FINAL)
    Q_PROPERTY(int deck_id MEMBER deck_id FINAL)
    Q_PROPERTY(int state MEMBER state FINAL)
    Q_PROPERTY(int incorrect_streak MEMBER incorrect_streak FINAL)
    Q_PROPERTY(int time_limit MEMBER time_limit FINAL)
    Q_PROPERTY(float difficulty MEMBER difficulty FINAL)
    Q_PROPERTY(QDateTime next_review MEMBER next_review FINAL)
    Q_PROPERTY(QDateTime created_at MEMBER created_at FINAL)
    Q_PROPERTY(QDateTime updated_at MEMBER updated_at FINAL)
    Q_PROPERTY(QString front MEMBER front FINAL)
    Q_PROPERTY(QString back MEMBER back FINAL)
};

} // namespace ui
