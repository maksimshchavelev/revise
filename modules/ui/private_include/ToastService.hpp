// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQml>                  // for QML_VALUE_TYPE, Q_GADGET
#include <QObject>                // for QObject
#include <QQueue>                 // for QQueue
#include <core/IToastService.hpp> // for IToastService

namespace ui {

/**
 * @brief A structure describing a single toast message
 */
struct ToastRequest {
    Q_GADGET
    QML_VALUE_TYPE(toastRequest)

public:
    enum class ToastType {
        INFO,    ///< Info toast
        SUCCESS, ///< Success toast
        ERROR    ///< Error toast
    };

    Q_ENUM(ToastType)

    Q_PROPERTY(QString header MEMBER header)
    Q_PROPERTY(QString message MEMBER message)
    Q_PROPERTY(ToastType type MEMBER type)

    QString   header;  ///< ToastRequest header
    QString   message; ///< ToastRequest message
    ToastType type;    ///< ToastRequest type
};

/// @brief ToastService wrapper for UI
class ToastService final : public QObject {
    Q_OBJECT

  public:
    ToastService(core::IToastService& toast_service, QObject* parent = nullptr);

    /// @brief Called when a toast message is closed to display a new one (if available)
    Q_INVOKABLE void closed();

  signals:
    void requested(ToastRequest toast);

  private:
    core::IToastService& m_toast_service;
};

} // namespace ui

Q_DECLARE_METATYPE(ui::ToastRequest)
