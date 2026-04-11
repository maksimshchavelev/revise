// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "SettingsError.hpp" // for errors
#include <expected>          // for std::expected

namespace core {

/// @brief Training Algorithm Settings
class AlgorithmSettings {
  public:
    virtual ~AlgorithmSettings() = default;

    /// @brief The learning speed affects how quickly the difficulty of the card changes
    virtual float learning_rate() const = 0;

    /// @brief Set learning rate (from 1 to 5)
    virtual std::expected<void, SettingsError> set_learning_rate(float rate) = 0;

    /// @brief The maximum number of days between reviews of the same card
    virtual int max_interval() const = 0;

    /// @brief Set maximum interval (from 1 to 365)
    virtual std::expected<void, SettingsError> set_max_interval(int interval) = 0;
};

} // namespace core