// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtGlobal> // for Qt macro
#include <optional> // for std::optional

#ifdef Q_OS_ANDROID
#include <QJniObject> // for QJniObject
#endif

namespace platform {

/// @breif A static class with helper constants for each platform
class Platform {
  public:
    /// @brief Returns true if the target platform supports multiple windows for a single application
    static consteval bool supports_multiple_windows() noexcept {
#ifdef Q_OS_ANDROID
        return false;
#else
        return true;
#endif
    }

    /// @brief Returns Android SDK version. Returns std::nullptr on other platforms
    static std::optional<int> android_sdk_version() noexcept {
#ifdef Q_OS_ANDROID
        return QJniObject::getStaticField<jint>("android/os/Build$VERSION", "SDK_INT");
#endif

        return std::nullopt;
    }
};

} // namespace platform
