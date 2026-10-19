// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQml/QtQml>        // for QML_ELEMENT and Q_GADGET
#include <core/BuildInfo.hpp> // for BuildInfo

namespace ui {

/// @brief Provides access to information about the application build, such as the version or compiler flags
struct BuildInfo {
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("BuildInfo cannot be created from QML, use C++ instance")

    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString compilerId READ compiler_id CONSTANT)
    Q_PROPERTY(QString compilerVersion READ compiler_version CONSTANT)
    Q_PROPERTY(QString targetArch READ target_arch CONSTANT)
    Q_PROPERTY(QString hostArch READ host_arch CONSTANT)
    Q_PROPERTY(QString buildType READ build_type CONSTANT)
    Q_PROPERTY(QString buildFlags READ build_flags CONSTANT)

    /// @brief Get application version as `QString`
    static QString version() {
        return QString("%1.%2.%3")
            .arg(core::REVISE_VERSION_MAJOR, //
                 core::REVISE_VERSION_MINOR, //
                 core::REVISE_VERSION_PATCH);
    }

    /// @brief Get compiler name
    static QString compiler_id() {
        return core::REVISE_BUILD_COMPILER_ID;
    }

    /// @brief Get compiler version
    static QString compiler_version() {
        return core::REVISE_BUILD_COMPILER_VERSION;
    }

    /// @brief Get target architecture
    static QString target_arch() {
        return core::REVISE_BUILD_TARGET_ARCH;
    }

    /// @brief Get host architecture
    static QString host_arch() {
        return core::REVISE_BUILD_HOST_ARCH;
    }

    /// @brief Get build type
    static QString build_type() {
        return core::REVISE_BUILD_TYPE;
    }

    /// @brief Get build flags
    static QString build_flags() {
        return core::REVISE_BUILD_FLAGS;
    }
};

} // namespace ui
