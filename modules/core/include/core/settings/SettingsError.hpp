// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

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
    QString         message;
};

} // namespace core