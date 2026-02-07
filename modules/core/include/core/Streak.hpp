// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QDateTime> // for QDateTime

namespace core {

/**
 * @brief Structure describes streak
 */
struct Streak {
    int       value;      ///< Number of days of streak
    QDateTime updated_at; ///< Last date and time streak was updated
};

} // namespace core