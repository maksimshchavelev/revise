// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "utils/Directory.hpp" // for header
#include <QDirIterator>

namespace utils {

std::expected<void, DirectoryError> utils::Directory::copy_recursively(const QString& source,
                                                                       const QString& destination) {
    QDir src_dir(source);
    QDir dest_dir(destination);

    if (!src_dir.exists()) {
        return std::unexpected(DirectoryError{DirectoryError::Error::SourceDirectoryNotExists,
                                              QString("The source directory '%1' does not exist").arg(source)});
    }

    QDirIterator it(source, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();

        QString relative_path = src_dir.relativeFilePath(it.filePath());
        QString destination_path = destination + "/" + relative_path;

        if (it.fileInfo().isDir()) {
            if (QDir().mkpath(destination_path)) {
                continue;
            } else {
                return std::unexpected(
                    DirectoryError{DirectoryError::Error::FailedToCreateDirectory,
                                   QString("Failed to create destination directory '%1'").arg(destination)});
            }
        }

        if (QFile(destination_path).exists()) {
            QFile::setPermissions(destination_path, QFile::ReadOwner | QFile::WriteOwner);

            if (!QFile(destination_path).remove()) {
                return std::unexpected(
                    DirectoryError{DirectoryError::Error::FailedToRemoveFile,
                               QString("Failed to remove existing destination file '%1'").arg(destination_path)});
            }
        }

        if (!QFile(it.filePath()).copy(destination_path)) {
            return std::unexpected(DirectoryError{
                DirectoryError::Error::FailedToCopyFile,
                QString("Failed to copy file from '%1' to '%2'").arg(it.filePath()).arg(destination_path)});
        }
    }

    return {};
}

} // namespace utils
