// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QQmlIncubator> // for QQmlIncubator

namespace ui {

/// @brief A QML widget that makes a page invisible immediately after it loads
class QmlIncubator final : public QQmlIncubator {
  public:
    void statusChanged(QQmlIncubator::Status status) override;
};

} // namespace ui
