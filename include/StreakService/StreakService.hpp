// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <Database/Database.hpp> // Database
#include <QObject>               // for QObject

namespace revise {

class StreakService : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(int streak READ streak NOTIFY streak_changed)

    /**
     * @brief StreakService constructor
     * @param driver Driver of the database
     * @param parent parent QObject
     */
    StreakService(Database& driver, QObject* parent = nullptr);

    /**
     * @brief Get current streak
     * @return Streak value
     */
    int streak();

    /**
     * @brief Set streak
     * @param streak Streak value
     * @return `std::expected<void, QString>`
     */
    std::expected<void, QString> set_streak(int streak);

    /**
     * @brief Reset streak if it is overdue
     * @return `std::expected<void, QString>`
     */
    std::expected<void, QString> reset_if_overdue();

    /**
     * @brief Reset streak
     * @return `std::expected<void, QString>`
     * @note Saves last update date
     */
    std::expected<void, QString> reset();

  signals:
    void streakChanged();

  private:
    Database& m_db;
};

} // namespace revise
