// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

namespace core {

/// @brief A class that provides information about the application version
class VersionSettings {
  public:
    virtual ~VersionSettings() = default;

    /// @brief Get application version in `VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH` format
    virtual QString application_version() const noexcept = 0;
};

} // namespace core