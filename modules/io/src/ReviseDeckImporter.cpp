// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReviseDeckImporter.hpp" // for ReviseDeckImporter
#include <QDir>                   // for QDir
#include <QSqlError>              // for QSqlError
#include <QSqlQuery>              // for QSqlQuery
#include <QStandardPaths>         // for QStandardPaths
#include <QUuid>                  // for QUuid
#include <quazip.h>               // for QuaZip
#include <quazipfile.h>           // for QuaZipFile
#include <utils/ScopeGuard.hpp>   // for utils::ScopeGuard

namespace io {

// Public method
core::IDeckImporter::Result<core::ImportResult> ReviseDeckImporter::import_from_file(const QString& path) const {
    core::ImportResult result;
    const QString      import_dir = QString("%1/revise_import_%2")
                                   .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                   .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));


    utils::ScopeGuard dir_guard([]() {},
                                [&import_dir]() {
                                    QDir(import_dir).removeRecursively(); // for autoremove import directory
                                });

    if (const auto res = unzip(path, import_dir); !res) {
        return std::unexpected(Error::extraction_failed(QString("Failed to unzip input file: %1").arg(res.error())));
    }

    // Open database
    QSqlDatabase      import_db;
    utils::ScopeGuard db_guard(
        [&import_dir, &import_db]() {
            // on enter
            import_db = QSqlDatabase::addDatabase("QSQLITE", "revise_export");
            import_db.setDatabaseName(QString("%1/db").arg(import_dir));
        },
        [&import_db]() {
            // on exit
            import_db.close();
            QSqlDatabase::removeDatabase("revise_export");
        });

    if (!import_db.open()) {
        return std::unexpected(Error::file_corrupted(
            QString("Failed to open db: %1, cause: %2").arg(import_db.databaseName(), import_db.lastError().text())));
    }

    // Read media
    if (const QDir media_dir(QString("%1/media").arg(import_dir)); QDir(media_dir).exists()) { // if media directory exists
        QMap<QString /* image name */, QByteArray /* mapped images */> mapped_images;
        QStringList files = media_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

        for (const auto& file : files) {
            const QString& file_path = QString("%1/%2").arg(media_dir.path()).arg(file);
            QFile          img(file_path);

            if (!img.open(QIODevice::ReadOnly)) {
                return std::unexpected(Error::file_corrupted(QString("Failed to open image file: %1").arg(file_path)));
            }

            mapped_images[file] = img.readAll();
            img.close();
        }

        // Append to res
        result.user_data = QVariant::fromValue(std::move(mapped_images));
    } else {
        qDebug() << "Skipping media directory (does not exists):" << media_dir.path();
    }

    // Query
    QSqlQuery q(import_db);

    // Import deck
    q.prepare(R"(
        SELECT * FROM decks
    )");

    if (!q.exec()) {
        return std::unexpected(
            Error::internal(QString("Failed to execute query %1, cause: %2").arg(q.lastQuery(), q.lastError().text())));
    }

    if (!q.next()) {
        return std::unexpected(Error::file_corrupted("'decks' table in import database in empty"));
    }

    result.deck_name = q.value("name").toString();
    result.deck_description = q.value("description").toString();
    result.time_limit = q.value("time_limit").toInt();
    result.new_limit = q.value("new_limit").toInt();
    result.consolidate_limit = q.value("consolidate_limit").toInt();
    result.incorrect_limit = q.value("incorrect_limit").toInt();

    // Import cards
    q.prepare(R"(
        SELECT * FROM cards
    )");

    if (!q.exec()) {
        return std::unexpected(
            Error::internal(QString("Failed to execute query %1, cause: %2").arg(q.lastQuery(), q.lastError().text())));
    }

    while (q.next()) {
        core::Card card;

        card.front = q.value("front").toString();
        card.back = q.value("back").toString();

        card.created_at = QDateTime::currentDateTime();
        card.updated_at = QDateTime::currentDateTime();
        card.next_review = QDateTime::currentDateTime();

        card.difficulty = 2.5f;
        card.interval = 0;
        card.incorrect_streak = 0;

        result.cards.push_back(std::move(card));
    }

    return result;
}

// Public method
QString ReviseDeckImporter::format_name() const {
    return "revise";
}

// Private method
std::expected<void, QString> ReviseDeckImporter::unzip(const QString& zip_path, const QString& target_dir) {
    QuaZip zip(zip_path);

    if (!zip.open(QuaZip::mdUnzip)) {
        return std::unexpected("Cannot open zip file " + zip_path);
    }

    QuaZipFile file(&zip);
    QDir       dir(target_dir);

    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        file.open(QIODevice::ReadOnly);

        QString out_path = dir.filePath(zip.getCurrentFileName());

        QFile out(out_path);

        if (const auto path = QFileInfo(out).absolutePath(); !QDir().mkpath(path)) {
            return std::unexpected(QString("unzip(): failed to make path: %1").arg(path));
        }

        if (!out.open(QIODevice::WriteOnly)) {
            return std::unexpected("unzip(): failed to open output file for writing");
        }

        out.write(file.readAll());

        file.close();
        out.close();
    }

    zip.close();
    return {};
}

} // namespace io
