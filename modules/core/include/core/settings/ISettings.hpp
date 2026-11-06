// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "AlgorithmSettings.hpp" // for AlgorithmSettings
#include "MainSettings.hpp"      // for MainSettings

namespace core {

/// @brief Interface for all settings
class ISettings : public AlgorithmSettings, public MainSettings {
  public:
    virtual ~ISettings() = default;
};

} // namespace core
