// Copyright 2025 Maksim Shchavelev

#include "ReviseDeckExporter.hpp"          // for ReviseDeckExporter header
#include <QDir>                            // for QDir
#include <QRegularExpressionMatchIterator> // for QRegularExpressionMatchIterator
#include <QSqlDatabase>                    // for QSqlDatabase
#include <QSqlError>                       // for QSqlError
#include <QSqlQuery>                       // for QSqlQuery
#include <QStandardPaths>                  // for QStandardPaths
#include <QUuid>                           // for QUuid
#include <quazip.h>                        // for QuaZip
#include <quazipfile.h>                    // for QuaZipFile
#include <utils/ScopeGuard.hpp>            // for ScopeGuard

namespace io {

// Public method
std::expected<void, QString> ReviseDeckExporter::export_to_file(const core::ExportData& data, const QString& path) {
    // temporary export dir
    QDir export_dir(QString("%1/revise_export_%2")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));

    if (!export_dir.mkdir(export_dir.path())) {
        return std::unexpected(QString("Failed to make temporary export directory: %1").arg(export_dir.path()));
    }

    utils::ScopeGuard _([&]() {}, [&]() { export_dir.removeRecursively(); });

    // export db
    QSqlDatabase export_db = QSqlDatabase::addDatabase("QSQLITE", "revise_export");
    export_db.setDatabaseName(QString("%1/db").arg(export_dir.path()));

    if (!export_db.open()) {
        return std::unexpected(QString("Failed to open export database: %1").arg(export_db.lastError().text()));
    }

    // Make tables
    QSqlQuery q(export_db);

    q.prepare(R"(
        CREATE TABLE decks (
        id INTEGER PRIMARY KEY,
        name TEXT UNIQUE,
        description TEXT,
        time_limit INTEGER DEFAULT 0,
        new_limit INTEGER DEFAULT 30,
        consolidate_limit INTEGER DEFAULT 30,
        incorrect_limit INTEGER DEFAULT 30)
    )");
    if (!q.exec()) {
        return std::unexpected(QString("Failed to create table 'decks' in export db. Query is: %1, cause is: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    q.prepare(R"(
        CREATE TABLE cards (
        id INTEGER PRIMARY KEY,
        front TEXT UNIQUE,
        back TEXT)
    )");
    if (!q.exec()) {
        return std::unexpected(QString("Failed to create table 'cards' in export db. Query is: %1, cause is: %2")
                                   .arg(q.lastQuery())
                                   .arg(q.lastError().text()));
    }

    if (!export_db.transaction()) {
        return std::unexpected(QString("Failed to begin export db transaction: %1").arg(export_db.lastError().text()));
    }

    // fill deck
    q.prepare(R"(
        INSERT INTO decks (name, description, time_limit, new_limit, consolidate_limit, incorrect_limit)
        VALUES (:name, :description, :time_limit, :new_limit, :consolidate_limit, :incorrect_limit)
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

    // fill cards
    for (const auto& card : data.cards) {
        q.prepare(R"(
            INSERT INTO cards (front, back)
            VALUES (:front, :back)
        )");
        q.bindValue(":front", strip_img_src_paths(card.front)); // strip html img
        q.bindValue(":back", strip_img_src_paths(card.back));   // strip html img

        if (!q.exec()) {
            return std::unexpected(QString("Failed to insert card into export db: %1").arg(q.lastError().text()));
        }
    }

    if (!export_db.commit()) {
        return std::unexpected(QString("Failed to commit export db transaction: %1").arg(export_db.lastError().text()));
    }

    export_db.close();
    QSqlDatabase::removeDatabase("revise_export");

    // export media
    if (!data.media_directory.isEmpty()) {
        QDir media_dir(data.media_directory);

        if (!media_dir.exists()) {
            return std::unexpected(QString("Media directory '%1' is not exists").arg(media_dir.path()));
        }

        // export media dir
        QDir().mkdir(QString("%1/media").arg(export_dir.path()));

        QStringList files = media_dir.entryList(QDir::Files);
        for (const QString& file : files) {
            QString src_name = media_dir.path() + QDir::separator() + file;
            QString dest_name = QString("%1/media/%2").arg(export_dir.path()).arg(file);

            if (!QFile::copy(src_name, dest_name)) {
                return std::unexpected(QString("Failed to copy media file %1 to %2").arg(src_name).arg(dest_name));
            }
        }
    }

    // compress
    const QString out = QString("%1/%2.rpkg").arg(path).arg(data.deck.name);

    if (auto res = compress_directory_to_zip(export_dir.path(), out); !res.has_value()) {
        return std::unexpected(res.error());
    }

    return {};
}

// Public method
QString ReviseDeckExporter::format_name() const noexcept {
    return "rpkg";
}

// Private method
std::expected<void, QString> ReviseDeckExporter::compress_directory_to_zip(const QString& source_dir_path,
                                                                           const QString& zip_file_path) {
    QDir source_dir(source_dir_path);
    if (!source_dir.exists()) {
        return std::unexpected(QString("Source directory does not exist: %1").arg(source_dir_path));
    }

    QuaZip zip(zip_file_path);
    if (!zip.open(QuaZip::mdCreate)) {
        return std::unexpected(QString("Failed to create ZIP archive: %1").arg(zip_file_path));
    }

    const QString base_path = source_dir.absolutePath();

    /**
     * @brief Recursively adds directory contents to the ZIP archive.
     */
    std::function<std::expected<void, QString>(const QDir&)> add_directory =
        [&](const QDir& current_dir) -> std::expected<void, QString> {
        const QFileInfoList entries =
            current_dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst);

        for (const QFileInfo& entry : entries) {
            if (entry.isDir()) {
                auto result = add_directory(QDir(entry.absoluteFilePath()));
                if (!result) {
                    return result;
                }
            } else if (entry.isFile()) {
                QFile input_file(entry.absoluteFilePath());
                if (!input_file.open(QIODevice::ReadOnly)) {
                    return std::unexpected(QString("Failed to open file: %1").arg(entry.absoluteFilePath()));
                }

                const QString relative_path = source_dir.relativeFilePath(entry.absoluteFilePath());

                QuaZipFile    zip_file(&zip);
                QuaZipNewInfo zip_info(relative_path, entry.absoluteFilePath());

                if (!zip_file.open(QIODevice::WriteOnly, zip_info)) {
                    return std::unexpected(QString("Failed to add file to ZIP: %1").arg(relative_path));
                }

                static constexpr qint64 buffer_size = 64 * 4096; // 256 KB
                QByteArray              buffer;
                buffer.resize(buffer_size);

                while (!input_file.atEnd()) {
                    const qint64 bytes_read = input_file.read(buffer.data(), buffer.size());
                    if (bytes_read < 0) {
                        zip_file.close();
                        return std::unexpected(QString("Failed to read file: %1").arg(entry.absoluteFilePath()));
                    }

                    if (zip_file.write(buffer.constData(), bytes_read) != bytes_read) {
                        zip_file.close();
                        return std::unexpected(QString("Failed to write file to ZIP: %1").arg(relative_path));
                    }
                }

                zip_file.close();
                input_file.close();

                if (zip_file.getZipError() != UNZ_OK) {
                    return std::unexpected(QString("ZIP error while writing file: %1").arg(relative_path));
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
        return std::unexpected(QString("ZIP archive finalization failed"));
    }

    return {};
}

// Private method
QString ReviseDeckExporter::strip_img_src_paths(const QString& html) {
    QString result = html;

    QRegularExpression img_src_regex(R"((<img[^>]*\bsrc\s*=\s*["'])([^"'>]+)(["']))",
                                     QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator it = img_src_regex.globalMatch(result);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        const QString full_match = match.captured(0);
        const QString prefix = match.captured(1);
        const QString src_value = match.captured(2);
        const QString suffix = match.captured(3);

        const int last_slash_index = src_value.lastIndexOf('/');
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
