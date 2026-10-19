// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

namespace core {

constexpr int REVISE_DB_SCHEMA_VERSION = 2;

// Version
constexpr int REVISE_VERSION_MAJOR = 1;
constexpr int REVISE_VERSION_MINOR = 3;
constexpr int REVISE_VERSION_PATCH = 3;

// Build info
inline const auto REVISE_BUILD_COMPILER_ID = "GNU";
inline const auto REVISE_BUILD_COMPILER_VERSION = "14.2.0";
inline const auto REVISE_BUILD_TARGET_ARCH = "x86_64";
inline const auto REVISE_BUILD_HOST_ARCH = "x86_64";
inline const auto REVISE_BUILD_TYPE = "Debug";
inline const auto REVISE_BUILD_FLAGS = "-O2 -g -O0";

// Qt info
inline const auto REVISE_QT_VERSION = "6.12.0";

}

