// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/IToastService.hpp>             // for IToastService

namespace ui {

/**
 * @brief A structure describing a single toast message
 */
struct ToastRequest {
    Q_GADGET
    QML_VALUE_TYPE(toastRequest)

  public:
    /// @brief Type of toast message
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

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("ToastService is provided by C++")

  public:
    explicit ToastService(core::IToastService& toast_service, QObject* parent = nullptr);

    /// @brief Called when a toast message is closed to display a new one (if available)
    Q_INVOKABLE void closed() const;

  signals:
    void requested(ToastRequest toast);

  private:
    core::IToastService& m_toast_service;
};

} // namespace ui
