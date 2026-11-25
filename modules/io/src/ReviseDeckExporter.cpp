// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReviseDeckExporter.hpp"

#include <QByteArray>
#include <QRegularExpressionMatch>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QUrl>
#include <QUuid>
#include <expected>
#include <functional>
#include <quazip.h>
#include <quazipfile.h>
#include <utils/ScopeGuard.hpp>

namespace io {

namespace {

/**
 * @brief Copies a local file to an Android content URI.
 *
 * The destination is accessed sequentially, so it does not need
 * to support random access operations required by ZIP creation.
 *
 * @param source_path Path to the local source file.
 * @param destination_uri Destination content URI.
 * @return Empty expected on success, otherwise an error description.
 */
std::expected<void, QString> copy_file_to_content_uri(const QString& source_path, const QString& destination_uri) {
    QFile source_file(source_path);

    if (!source_file.open(QIODevice::ReadOnly)) {
        return std::unexpected(QString("Failed to open source archive '%1': %2").arg(source_path).arg(source_file.errorString()));
    }

    QFile destination_file(destination_uri);

    if (!destination_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return std::unexpected(
            QString("Failed to open destination URI '%1': %2").arg(destination_uri).arg(destination_file.errorString()));
    }

    static constexpr qint64 buffer_size = 64 * 4096;

    QByteArray buffer;
    buffer.resize(buffer_size);

    while (true) {
        const qint64 bytes_read = source_file.read(buffer.data(), buffer.size());

        if (bytes_read < 0) {
            return std::unexpected(
                QString("Failed to read source archive '%1': %2").arg(source_path).arg(source_file.errorString()));
        }

        if (bytes_read == 0) {
            if (source_file.error() != QFileDevice::NoError) {
                return std::unexpected(
                    QString("Failed to read source archive '%1': %2").arg(source_path).arg(source_file.errorString()));
            }

            break;
        }

        qint64 total_written = 0;

        while (total_written < bytes_read) {
            const qint64 bytes_written = destination_file.write(buffer.constData() + total_written, bytes_read - total_written);

            if (bytes_written <= 0) {
                return std::unexpected(
                    QString("Failed to write destination URI '%1': %2").arg(destination_uri).arg(destination_file.errorString()));
            }

            total_written += bytes_written;
        }
    }

    if (!destination_file.flush()) {
        return std::unexpected(
            QString("Failed to flush destination URI '%1': %2").arg(destination_uri).arg(destination_file.errorString()));
    }

    destination_file.close();

    if (destination_file.error() != QFileDevice::NoError) {
        return std::unexpected(
            QString("Failed to finalize destination URI '%1': %2").arg(destination_uri).arg(destination_file.errorString()));
    }

    return {};
}

} // namespace

/**
 * @brief Exports a deck into a Revise package.
 *
 * Local file destinations are handled directly by QuaZip.
 * Android content URIs are handled by first creating a local archive
 * and then copying it into the selected document.
 *
 * @param data Deck data to export.
 * @param path Destination filesystem path or URL.
 * @return Empty expected on success, otherwise an error description.
 */
std::expected<void, QString> ReviseDeckExporter::export_to_file(const core::ExportData& data, const QString& path) {
    if (path.trimmed().isEmpty()) {
        return std::unexpected(QString("Export destination path is empty"));
    }

    const QUrl    destination_url(path);
    const QString scheme = destination_url.scheme().toLower();

    const bool is_content_uri = scheme == QStringLiteral("content");

#ifdef Q_OS_WIN
    const bool is_windows_drive_path =
        path.size() >= 3 && path.at(1) == QLatin1Char(':') && (path.at(2) == QLatin1Char('/') || path.at(2) == QLatin1Char('\\'));
#else
    const bool is_windows_drive_path = false;
#endif

    QString local_destination_path;
    QString content_destination_uri;

    if (destination_url.isLocalFile()) {
        local_destination_path = destination_url.toLocalFile();

        if (local_destination_path.isEmpty()) {
            return std::unexpected(QString("Failed to convert destination URL to a local path: %1").arg(path));
        }
    } else if (is_content_uri) {
        if (!destination_url.isValid()) {
            return std::unexpected(QString("Invalid destination content URI: %1").arg(path));
        }

        content_destination_uri = destination_url.toString(QUrl::FullyEncoded);
    } else if (scheme.isEmpty() || is_windows_drive_path) {
        local_destination_path = path;
    } else {
        return std::unexpected(QString("Unsupported destination URL scheme '%1': %2").arg(scheme).arg(path));
    }

    /**
     * @brief Create a temporary working directory.
     *
     * Its structure is:
     *
     * revise_export_XXXXXX/
     *     package/
     *         db
     *         media/
     *     export.rpkg
     *
     * The archive is deliberately located outside package/,
     * otherwise it could be included in itself during compression.
     */

    const QString temp_location = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    if (temp_location.isEmpty()) {
        return std::unexpected(QString("Failed to obtain temporary directory"));
    }

    if (!QDir().mkpath(temp_location)) {
        return std::unexpected(QString("Failed to create temporary directory: %1").arg(temp_location));
    }

    QTemporaryDir working_dir(QDir(temp_location).filePath(QStringLiteral("revise_export_XXXXXX")));

    if (!working_dir.isValid()) {
        return std::unexpected(QString("Failed to create temporary export directory: %1").arg(working_dir.path()));
    }

    const QString export_dir_path = QDir(working_dir.path()).filePath(QStringLiteral("package"));

    if (!QDir().mkpath(export_dir_path)) {
        return std::unexpected(QString("Failed to create export package directory: %1").arg(export_dir_path));
    }

    /**
     * @brief Create the export SQLite database.
     *
     * A unique connection name prevents collisions with other
     * SQL connections in the application.
     */

    const QString connection_name = QStringLiteral("revise_export_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    {
        QSqlDatabase export_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

        utils::ScopeGuard db_guard([]() {},
                                   [&]() {
                                       export_db.close();

                                       // Release this connection handle before removing the connection.
                                       export_db = QSqlDatabase{};

                                       QSqlDatabase::removeDatabase(connection_name);
                                   });

        export_db.setDatabaseName(QDir(export_dir_path).filePath(QStringLiteral("db")));

        if (!export_db.open()) {
            return std::unexpected(QString("Failed to open export database: %1").arg(export_db.lastError().text()));
        }

        {
            QSqlQuery q(export_db);

            // Create decks table.
            q.prepare(R"(
                CREATE TABLE decks (
                    id INTEGER PRIMARY KEY,
                    name TEXT UNIQUE,
                    description TEXT,
                    time_limit INTEGER DEFAULT 0,
                    new_limit INTEGER DEFAULT 30,
                    consolidate_limit INTEGER DEFAULT 30,
                    incorrect_limit INTEGER DEFAULT 30
                )
            )");

            if (!q.exec()) {
                return std::unexpected(QString("Failed to create table 'decks' in export db. "
                                               "Query is: %1, cause is: %2")
                                           .arg(q.lastQuery())
                                           .arg(q.lastError().text()));
            }

            // Create cards table.
            q.prepare(R"(
                CREATE TABLE cards (
                    id INTEGER PRIMARY KEY,
                    front TEXT UNIQUE,
                    back TEXT
                )
            )");

            if (!q.exec()) {
                return std::unexpected(QString("Failed to create table 'cards' in export db. "
                                               "Query is: %1, cause is: %2")
                                           .arg(q.lastQuery())
                                           .arg(q.lastError().text()));
            }

            if (!export_db.transaction()) {
                return std::unexpected(QString("Failed to begin export db transaction: %1").arg(export_db.lastError().text()));
            }

            // Insert deck.

            q.prepare(R"(
                INSERT INTO decks (
                    name,
                    description,
                    time_limit,
                    new_limit,
                    consolidate_limit,
                    incorrect_limit
                )
                VALUES (
                    :name,
                    :description,
                    :time_limit,
                    :new_limit,
                    :consolidate_limit,
                    :incorrect_limit
                )
            )");

            q.bindValue(":name", data.deck.name);
            q.bindValue(":description", data.deck.description);
            q.bindValue(":time_limit", data.deck.time_limit);
            q.bindValue(":new_limit", data.deck.new_limit);
            q.bindValue(":consolidate_limit", data.deck.review_limit);
            q.bindValue(":incorrect_limit", data.deck.incorrect_limit);

            if (!q.exec()) {
                return std::unexpected(QString("Failed to insert deck into export db: %1").arg(q.lastError().text()));
            }

            // Insert cards.

            q.prepare(R"(
                INSERT INTO cards (front, back)
                VALUES (:front, :back)
            )");

            for (const auto& card : data.cards) {
                q.bindValue(":front", strip_img_src_paths(card.front));
                q.bindValue(":back", strip_img_src_paths(card.back));

                if (!q.exec()) {
                    return std::unexpected(QString("Failed to insert card into export db: %1").arg(q.lastError().text()));
                }
            }

            if (!export_db.commit()) {
                return std::unexpected(QString("Failed to commit export db transaction: %1").arg(export_db.lastError().text()));
            }
        }

        // QSqlQuery has been destroyed at this point.
        export_db.close();
    }

    // Export media.

    if (!data.media_directory.isEmpty()) {
        QDir media_dir(data.media_directory);

        if (!media_dir.exists()) {
            return std::unexpected(QString("Media directory '%1' does not exist").arg(media_dir.path()));
        }

        const QString export_media_path = QDir(export_dir_path).filePath(QStringLiteral("media"));

        if (!QDir().mkpath(export_media_path)) {
            return std::unexpected(QString("Failed to create export media directory: %1").arg(export_media_path));
        }

        const QStringList files = media_dir.entryList(QDir::Files, QDir::Name);

        for (const QString& file : files) {
            const QString src_name = media_dir.filePath(file);

            const QString dest_name = QDir(export_media_path).filePath(file);

            if (!QFile::copy(src_name, dest_name)) {
                return std::unexpected(QString("Failed to copy media file %1 to %2").arg(src_name).arg(dest_name));
            }
        }
    }

    /**
     * @brief Select the archive destination.
     *
     * Desktop:
     *     Write directly to the selected local destination.
     *
     * Android content URI:
     *     Write to a temporary local file first.
     */

    const QString zip_file_path =
        is_content_uri ? QDir(working_dir.path()).filePath(QStringLiteral("export.rpkg")) : local_destination_path;

    if (auto res = compress_directory_to_zip(export_dir_path, zip_file_path); !res.has_value()) {
        return std::unexpected(res.error());
    }

    // Copy the finished archive into Android Storage Access Framework.

    if (is_content_uri) {
        if (auto res = copy_file_to_content_uri(zip_file_path, content_destination_uri); !res.has_value()) {
            return std::unexpected(res.error());
        }
    }

    return {};
}

/**
 * @brief Returns the export format name.
 */
QString ReviseDeckExporter::format_name() const noexcept {
    return QStringLiteral("rpkg");
}

/**
 * @brief Compresses a directory into a ZIP archive.
 *
 * The ZIP archive is created using a local filesystem path.
 *
 * @param source_dir_path Directory to compress.
 * @param zip_file_path Destination ZIP file path.
 * @return Empty expected on success, otherwise an error description.
 */
std::expected<void, QString> ReviseDeckExporter::compress_directory_to_zip(const QString& source_dir_path,
                                                                           const QString& zip_file_path) {
    QDir source_dir(source_dir_path);

    if (!source_dir.exists()) {
        return std::unexpected(QString("Source directory does not exist: %1").arg(source_dir_path));
    }

    if (zip_file_path.isEmpty()) {
        return std::unexpected(QString("ZIP destination path is empty"));
    }

    QuaZip zip(zip_file_path);

    if (!zip.open(QuaZip::mdCreate)) {
        return std::unexpected(
            QString("Failed to create ZIP archive '%1'. QuaZip error: %2").arg(zip_file_path).arg(zip.getZipError()));
    }

    /**
     * @brief Recursively adds directory contents to the ZIP archive.
     */
    std::function<std::expected<void, QString>(const QDir&)> add_directory =
        [&](const QDir& current_dir) -> std::expected<void, QString> {
        const QFileInfoList entries = current_dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst);

        for (const QFileInfo& entry : entries) {
            if (entry.isDir()) {
                const auto result = add_directory(QDir(entry.absoluteFilePath()));

                if (!result) {
                    return result;
                }
            } else if (entry.isFile()) {
                QFile input_file(entry.absoluteFilePath());

                if (!input_file.open(QIODevice::ReadOnly)) {
                    return std::unexpected(
                        QString("Failed to open file '%1': %2").arg(entry.absoluteFilePath()).arg(input_file.errorString()));
                }

                const QString relative_path = source_dir.relativeFilePath(entry.absoluteFilePath());

                QuaZipFile    zip_file(&zip);
                QuaZipNewInfo zip_info(relative_path, entry.absoluteFilePath());

                if (!zip_file.open(QIODevice::WriteOnly, zip_info)) {
                    return std::unexpected(QString("Failed to add file to ZIP '%1'. QuaZip error: %2")
                                               .arg(relative_path)
                                               .arg(zip_file.getZipError()));
                }

                static constexpr qint64 buffer_size = 64 * 4096;

                QByteArray buffer;
                buffer.resize(buffer_size);

                while (true) {
                    const qint64 bytes_read = input_file.read(buffer.data(), buffer.size());

                    if (bytes_read < 0) {
                        zip_file.close();

                        return std::unexpected(
                            QString("Failed to read file '%1': %2").arg(entry.absoluteFilePath()).arg(input_file.errorString()));
                    }

                    if (bytes_read == 0) {
                        break;
                    }

                    qint64 total_written = 0;

                    while (total_written < bytes_read) {
                        const qint64 bytes_written =
                            zip_file.write(buffer.constData() + total_written, bytes_read - total_written);

                        if (bytes_written <= 0) {
                            zip_file.close();

                            return std::unexpected(QString("Failed to write file to ZIP '%1'. "
                                                           "QuaZip error: %2")
                                                       .arg(relative_path)
                                                       .arg(zip_file.getZipError()));
                        }

                        total_written += bytes_written;
                    }
                }

                zip_file.close();
                input_file.close();

                if (zip_file.getZipError() != UNZ_OK) {
                    return std::unexpected(
                        QString("ZIP error while writing file '%1': %2").arg(relative_path).arg(zip_file.getZipError()));
                }
            }
        }

        return {};
    };

    auto result = add_directory(source_dir);

    zip.close();

    if (!result) {
        return result;
    }

    if (zip.getZipError() != UNZ_OK) {
        return std::unexpected(QString("ZIP archive finalization failed. QuaZip error: %1").arg(zip.getZipError()));
    }

    return {};
}

/**
 * @brief Removes local paths from image src attributes in HTML.
 *
 * Only the filename is retained.
 *
 * @param html Original HTML.
 * @return Modified HTML.
 */
QString ReviseDeckExporter::strip_img_src_paths(const QString& html) {
    QString result = html;

    const QRegularExpression img_src_regex(R"((<img[^>]*\bsrc\s*=\s*["'])([^"'>]+)(["']))",
                                           QRegularExpression::CaseInsensitiveOption);

    const QRegularExpressionMatchIterator it = img_src_regex.globalMatch(result);

    QRegularExpressionMatchIterator matches = it;

    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();

        const QString full_match = match.captured(0);
        const QString prefix = match.captured(1);
        const QString src_value = match.captured(2);
        const QString suffix = match.captured(3);

        const qsizetype last_slash_index = src_value.lastIndexOf('/');

        if (last_slash_index == -1) {
            continue;
        }

        const QString file_name = src_value.mid(last_slash_index + 1);

        const QString replacement = prefix + file_name + suffix;

        result.replace(full_match, replacement);
    }

    return result;
}

} // namespace io
