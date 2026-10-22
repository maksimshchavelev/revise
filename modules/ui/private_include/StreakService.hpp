// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <../../core/include/core/service/IStreakService.hpp>
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE

namespace ui {

/// @brief UI adapter over core::IStreakService
class StreakService final : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("StreakService is provided by C++")

  public:
    Q_PROPERTY(int streak READ streak NOTIFY streakChanged FINAL)
    Q_PROPERTY(bool updatedToday READ updated_today NOTIFY updatedTodayChanged FINAL)

    explicit StreakService(core::IStreakService& streak_service, QObject* parent = nullptr);

  signals:
    void streakChanged();
    void updatedTodayChanged();

  private:
    /// @brief Get current streak
    int streak() const noexcept;

    /// @brief Find out if the streak has been updated today (true if yes)
    bool updated_today() const noexcept;

    /// @brief Loads streak async
    void load();

    core::IStreakService&    m_streak_service;
    core::Streak::value_type m_streak{0};
    bool                     m_updated_today{false};
};

} // namespace ui