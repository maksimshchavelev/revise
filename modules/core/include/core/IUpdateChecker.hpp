// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime> // for QDateTime
#include <QFuture>   // for QFuture
#include <expected>  // for std::expected

namespace core {

/// @brief Checks for updates on the remote server. The source of the updates depends on the implementation
class IUpdateChecker {
  public:
    virtual ~IUpdateChecker() = default;

    /// @brief An error occurred while checking for updates
    struct Error {
        /// @brief Error Kind
        enum Kind {
            NetworkUnavailable, ///< The network is unavailable
            Timeout,            ///< The timeout for the server's response has been exceeded
            NotFound,           ///< The server returned an HTTP status code 404
            ServerError,        ///< Server-side Error 50x
            ParsingError,       ///< Error parsing the response from the server
            InternalError,      ///< Internal Error
        };

        Kind    kind;    ///< Error kind
        QString message; ///< Error message

        /// @brief Make `NetworkUnavailable` with custom message
        static Error network_unavailable(QString message) {
            return Error{.kind = NetworkUnavailable, .message = std::move(message)};
        }

        /// @brief Make `Timeout` with custom message
        static Error timeout(QString message) {
            return Error{.kind = Timeout, .message = std::move(message)};
        }

        /// @brief Make `NotFound` with custom message
        static Error not_found(QString message) {
            return Error{.kind = NotFound, .message = std::move(message)};
        }

        /// @brief Make `ServerError` with custom message
        static Error server_error(QString message) {
            return Error{.kind = ServerError, .message = std::move(message)};
        }

        /// @brief Make `InternalError` with custom message
        static Error internal_error(QString message) {
            return Error{.kind = InternalError, .message = std::move(message)};
        }
    };

    /// @brief Information about an available update
    struct UpdateInfo {
        QString   version;       ///< Update Version
        QString   release_notes; ///< Release notes
        QString   url;           ///< Link to page with update
        QDateTime published_at;  ///< When release was published
    };

    using ErrorKind = Error::Kind;
    template <typename T = void> using Result = std::expected<T, Error>;

    /// @brief Check for updates
    /// @return Optional `UpdateInfo` (there may not be an update) or an error
    [[nodiscard]] virtual QFuture<Result<std::optional<UpdateInfo>>> check_for_updates() const = 0;
};

} // namespace core
