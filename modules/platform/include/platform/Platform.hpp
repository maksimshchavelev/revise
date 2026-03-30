// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

namespace platform {

/// @breif A static class with helper constants for each platform
class Platform {
  public:
    /// @brief Returns true if the target platform supports multiple windows for a single application
    static consteval bool supports_multiple_windows() {
#ifdef Q_OS_ANDROID
        return false;
#else
        return true;
#endif
    }
};

} // namespace platform
