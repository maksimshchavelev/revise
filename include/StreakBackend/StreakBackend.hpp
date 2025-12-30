// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Class controlling the streak

#pragma once

#include <Database/Database.hpp>
#include <QObject> // for QObject

namespace revise {
class StreakBackend : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(quint32 streak READ streak NOTIFY streak_updated)
    Q_PROPERTY(bool isUpdatedToday READ is_updated_today NOTIFY is_updated_today_changed)

    StreakBackend(Database& db, QObject* parent = nullptr);

    /**
     * @brief Get streak
     * @return streak
     */

    quint32 streak() const noexcept;

    /**
     * @brief Set streak
     * @param streak Streak in days
     * @note In case of an error, the streak does not change. See debug output
     */
    void set_streak(quint32 streak) noexcept;

    /**
     * @brief If the streak was last updated the day before yesterday, reset it to zero.
     * @brief If the streak was updated yesterday or today, the method does nothing.
     * @note In case of an error, the streak does not change. See debug output
     */
    void reset_streak_if_overdue() noexcept;

    /**
     * @brief Is streak updated today?
     * @return `true if streak is updated today`
     * @note Returns false if an error occurred
     */
    bool is_updated_today() noexcept;

    /**
     * @brief Resets streak without updating date
     */
    void reset() noexcept;

  signals:
    void streak_updated();
    void is_updated_today_changed();

  private:
    quint32   m_streak;
    Database& m_db;
};
} // namespace revise
