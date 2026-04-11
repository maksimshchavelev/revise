// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "AlgorithmSettings.hpp" // for AlgorithmSettings

namespace core {

/// @brief Interface for all settings
class ISettings : public AlgorithmSettings {
  public:
    virtual ~ISettings() = default;
};

} // namespace core