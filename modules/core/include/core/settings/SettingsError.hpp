// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>
#include <system_error>

namespace core {

/// @brief Error handling the settings class
struct SettingsError {
    enum class Kind {
        InvalidValue,   ///< Invalid value (e.g., an empty string)
        OutOfBounds,    ///< The value is out of bounds
        BackendFailure, ///< Implementation error
        Unknown         ///< Unknown failure
    };

    Kind            kind{Kind::Unknown};
    std::error_code cause;
    QString         message;
};

} // namespace core