// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "UIExport.hpp" // for UI_EXPORT macro
#include <QObject>      // for QObject
#include <atomic>       // for std::atomic_bool

namespace ui {

/// @brief This class is used to display the loading screen. For example, when the app is launching
class UI_EXPORT LoadingScreen final : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

  public:
    LoadingScreen(QObject* parent = nullptr);

    void set_visible(bool visible) noexcept;

    bool visible() const noexcept;

  signals:
    void visibleChanged();

  private:
    std::atomic_bool m_visible{false}; ///< Is loading screen visible
};

} // namespace ui
