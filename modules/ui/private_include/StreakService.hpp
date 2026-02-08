// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                 // for QObject
#include <core/IStreakService.hpp> // for core::IStreakService

namespace ui {

/**
 * @brief UI adapter over core::IStreakService
 */
class StreakService final : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(int streak READ get NOTIFY streakChanged FINAL)
    Q_PROPERTY(bool updatedToday READ updated_today NOTIFY updatedTodayChanged FINAL)

    StreakService(core::IStreakService& streak_service, QObject* parent = nullptr);

  signals:
    void streakChanged();
    void updatedTodayChanged();

  private:
    /// Get current streak
    int get();

    /// Find out if the streak has been updated today (true if yes)
    bool updated_today();

    core::IStreakService& m_streak_service;
};

} // namespace ui