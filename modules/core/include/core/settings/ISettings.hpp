// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "AlgorithmSettings.hpp" // for AlgorithmSettings
#include "VersionSettings.hpp"   // for VersionSettings

namespace core {

/// @brief Interface for all settings
class ISettings : public AlgorithmSettings, public VersionSettings {
  public:
    virtual ~ISettings() = default;
};

} // namespace core