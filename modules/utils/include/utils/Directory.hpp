// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>  // for QString
#include <expected> // for std::expected

namespace utils {

/// @brief This section describes the various errors that may occur when working with directory tools
struct DirectoryError {
    enum class Error {
        CopyError,                ///< Copy error
        SourceDirectoryNotExists, ///< The source directory does not exist
        FailedToCreateDirectory,  ///< Failed to create directory
        FailedToRemoveFile,       ///< Failed to remove file
        FailedToCopyFile          ///< Failed to copy file
    };

    Error   error;       ///< Error
    QString description; ///< Error description
};

/**
 * @brief Class with utilities for working with application assets
 */
class Directory {
  public:
    /// @brief Recursively copies a directory
    static std::expected<void, DirectoryError> copy_recursively(const QString& source, const QString& destination);
};

} // namespace utils
