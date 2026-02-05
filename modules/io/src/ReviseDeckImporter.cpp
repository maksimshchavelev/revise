// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ReviseDeckImporter.hpp" // for ReviseDeckImporter
#include <QDir>                   // for QDir
#include <QSqlDatabase>           // for QSqlDatabase
#include <QSqlError>              // for QSqlError
#include <QSqlQuery>              // for QSqlQuery
#include <QStandardPaths>         // for QStandardPaths
#include <QUuid>                  // for QUuid
#include <utils/ScopeGuard.hpp>   // for utils::ScopeGuard
#include <quazip.h>               // for QuaZip
#include <quazipfile.h>           // for QuaZipFile

namespace io {

// Public method
std::expected<core::ImportResult, QString> ReviseDeckImporter::import_from_file(const QString& path) {
    core::ImportResult  res;
    const QString import_dir = QString("%1/revise_import_%2")
                                   .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                   .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));


    utils::ScopeGuard dir_guard([]() {},
                         [&import_dir]() {
                             QDir(import_dir).removeRecursively(); // for autoremove import directory
                         });

    // Unzip data
    if (auto res = unzip(path, import_dir); !res.has_value()) {
        return std::unexpected(res.error());
    }

    // Open database
    QSqlDatabase import_db;
    utils::ScopeGuard   db_guard(
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
        return std::unexpected(QString("Failed to open db: %1, cause: %2")
                                   .arg(import_db.databaseName())
                                   .arg(import_db.lastError().text()));
    }

    // Read media
    if (const QDir media_dir(QString("%1/media").arg(import_dir));
        QDir(media_dir).exists()) { // if media directory exists
        QMap<QString /* image name */, QByteArray /* mapped images */> mapped_images;
        QStringList files = media_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

        for (const auto& file : files) {
            const QString& file_path = QString("%1/%2").arg(media_dir.path()).arg(file);
            QFile          img(file_path);
            if (!img.open(QIODevice::ReadOnly)) {
                return std::unexpected(QString("Failed to open image file: %1").arg(file_path));
            }

            mapped_images[file] = img.readAll();
            img.close();
        }

        // Append to res
        res.user_data = QVariant::fromValue(std::move(mapped_images));
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
            QString("Failed to execure query %1, cause: %2").arg(q.lastQuery()).arg(q.lastError().text()));
    }

    if (!q.next()) {
        return std::unexpected("decks table in import db is empty");
    }

    res.deck_name = q.value("name").toString();
    res.deck_description = q.value("description").toString();
    res.time_limit = q.value("time_limit").toInt();
    res.new_limit = q.value("new_limit").toInt();
    res.consolidate_limit = q.value("consolidate_limit").toInt();
    res.incorrect_limit = q.value("incorrect_limit").toInt();

    // Import cards
    q.prepare(R"(
        SELECT * FROM cards
    )");

    if (!q.exec()) {
        return std::unexpected(
            QString("Failed to execure query %1, cause: %2").arg(q.lastQuery()).arg(q.lastError().text()));
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

        res.cards.push_back(std::move(card));
    }

    return res;
}

// Public method
QString ReviseDeckImporter::format_name() const noexcept {
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
        QDir().mkpath(QFileInfo(out).absolutePath());

        out.open(QIODevice::WriteOnly);
        out.write(file.readAll());

        file.close();
        out.close();
    }

    zip.close();
    return {};
}

} // namespace io
