// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Notifiable.hpp" // for Notifiable
#include "Streak.hpp"     // for Streak
#include <QFuture>        // for QFuture
#include <expected>       // for std::expected

namespace core {

/**
 * @brief An interface that abstracts working with streak storage. Through this class, you can get the current streak
 * and save it.
 */
class IStreakService : public Notifiable {
  public:
    EVENT(updated, int value) /// streak updated. Stores new streak value
    EVENT(reset)              /// streak reset

    /// @brief Streak service error
    struct Error {
        enum class Kind {
            InvalidValue, ///< Invalid streak value
            Unavailable,  ///< Service is unavailable
            InternalError ///< Internal error
        };

        Kind    kind;
        QString message;

        static Error invalid_value(QString message) {
            return Error{.kind = Kind::InvalidValue, .message = std::move(message)};
        }

        static Error unavailable(QString message) {
            return Error{.kind = Kind::Unavailable, .message = std::move(message)};
        }

        static Error internal_error(QString message) {
            return Error{.kind = Kind::InternalError, .message = std::move(message)};
        }
    };

    using ErrorKind = Error::Kind;
    template <typename T> using Result = std::expected<T, Error>;

    virtual ~IStreakService() = default;

    /// @brief Get the current streak
    virtual QFuture<Result<Streak::value_type>> get() const = 0;

    /// Set the streak
    virtual QFuture<Result<void>> set(Streak::value_type streak) = 0;

    /// @brief Resets the streak if it is overdue. How exactly the overdue criterion is determined is determined by the
    /// implementation.
    virtual QFuture<Result<void>> reset_if_overdue() = 0;

    /// @brief Check if your streak has expired
    virtual QFuture<Result<bool>> overdue() const = 0;

    /// @brief Updates the streak (increase by 1). If the strike has already been updated today, it does nothing.
    virtual QFuture<Result<void>> update() = 0;

    /// @brief @brief Check if the streak has been updated today
    virtual QFuture<Result<bool>> updated_today() const = 0;
};

} // namespace core