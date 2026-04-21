// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Streak.hpp" // for Streak
#include <expected>   // for std::expected

namespace core {

/**
 * @brief An interface that abstracts working with streak storage. Through this class, you can get the current streak
 * and save it.
 */
class IStreakStorage {
  public:
    /// @brief Describes streak storage errors
    struct Error {
        enum class Kind {
            Unavailable,  ///< Storage unavailable
            MissingValue, ///< Value doesn't exist
            InternalError ///< Internal error
        };

        Kind    kind;
        QString message;

        static Error unavailable(QString message) {
            return Error{.kind = Kind::Unavailable, .message = std::move(message)};
        }

        static Error missing_value(QString message) {
            return Error{.kind = Kind::MissingValue, .message = std::move(message)};
        }

        static Error internal_error(QString message) {
            return Error{.kind = Kind::InternalError, .message = std::move(message)};
        }
    };

    using ErrorKind = Error::Kind;
    template <typename T> using Result = std::expected<T, Error>;

    virtual ~IStreakStorage() = default;

    /**
     * @brief Saves the current streak
     * @param streak The `core::Streak` structure describing a streak
     */
    virtual Result<void> save(const Streak streak) = 0;

    /**
     * @brief Reads the current streak
     */
    virtual Result<Streak> read() = 0;
};

} // namespace core