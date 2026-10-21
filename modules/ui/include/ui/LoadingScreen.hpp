// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "UIExport.hpp"                       // for UI_EXPORT macro
#include <QMutex>                             // for QMutex
#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <atomic>                             // for std::atomic_bool

namespace ui {

/// @brief This class is used to display the loading screen. For example, when the app is launching
class UI_EXPORT LoadingScreen final : public QObject {
    Q_OBJECT

    QML_NAMED_ELEMENT(LoadingScreenService)
    QML_SINGLETON
    QML_UNCREATABLE("LoadingScreenService is provided by C++")

    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

  public:
    explicit LoadingScreen(QObject* parent = nullptr);

    /// @brief Set loading screen visibility
    void set_visible(bool visible) noexcept;

    /// @brief Is loading screen visible
    bool visible() const noexcept;

    /// @brief Set loading screen description
    void set_description(QString description);

    /// @brief Get loading screen description
    QString description() const;

  signals:
    void visibleChanged();
    void descriptionChanged();

  private:
    std::atomic_bool m_visible{false}; ///< Is loading screen visible
    QString          m_description;    ///< What's loading now?

    mutable QMutex m_mutex;
};

} // namespace ui
