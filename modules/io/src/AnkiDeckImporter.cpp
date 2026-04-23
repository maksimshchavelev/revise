// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "AnkiDeckImporter.hpp"            // for AnkiDeckImporter
#include <QDir>                            // for QDir
#include <QRegularExpressionMatchIterator> // for QRegularExpressionMatchIterator
#include <QSqlError>                       // for QSqlError
#include <QSqlQuery>                       // for QSqlQuery
#include <QStandardPaths>                  // for QStandardPaths
#include <QUuid>                           // for QUuid
#include <quazip.h>                        // for QuaZip
#include <quazipfile.h>                    // for QuaZipFile
#include <utils/ScopeGuard.hpp>            // for utils::ScopeGuard
#include <zstd.h>                          // for ZSTD

namespace io {

// Public method
std::expected<core::ImportResult, QString> AnkiDeckImporter::import_from_file(const QString& path) {
    core::ImportResult res;

    const QString base_temp_dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString import_dir = base_temp_dir + "/anki_import_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString db_path = import_dir + "/collection";
    const QString unpacked_media_path = import_dir + "/unpacked_media";
    QMap<QString /* image name */, QByteArray /* binary image */> mapped_images;

    QDir().mkdir(import_dir);

    QSqlDatabase anki_db = QSqlDatabase::addDatabase("QSQLITE", "anki_import");
    anki_db.setDatabaseName(db_path);

    utils::ScopeGuard _([]() {},
                        [&]() {
                            QDir(import_dir).removeRecursively();
                            anki_db.close();
                            QSqlDatabase::removeDatabase("anki_import");
                        });

    // Unzip .apkg
    if (auto res = unzip(path, import_dir); !res.has_value()) {
        return std::unexpected(res.error());
    }

    // Unzip collection.anki21b
    if (auto res = decompress_zstd_file(import_dir + "/collection.anki21b", db_path); !res.has_value()) {
        return std::unexpected(res.error());
    }

    if (!QFile::exists(db_path)) {
        return std::unexpected(QString("DB %1 not found").arg(db_path));
    }

    // Unzip media
    if (auto res = decompress_zstd_file(import_dir + "/media", unpacked_media_path); !res.has_value()) {
        return std::unexpected(res.error());
    }

    if (!QFile::exists(unpacked_media_path)) {
        return std::unexpected(QString("Unpacked media '%1' not found").arg(unpacked_media_path));
    }

    // Map media
    for (const auto& [image_name, archive_name] : map_images(unpacked_media_path).toStdMap()) {
        const QString archive_path = QString("%1/%2").arg(import_dir).arg(archive_name);
        const QString unpacked_path = QString("%1/unpacked_%2").arg(import_dir).arg(archive_name);
        if (auto res = decompress_zstd_file(archive_path, unpacked_path); !res.has_value()) {
            return std::unexpected(res.error());
        }

        QFile file(unpacked_path);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            mapped_images[image_name] = std::move(data);
        } else {
            return std::unexpected(QString("Failed to read unpacked image file '%1'").arg(unpacked_path));
        }
    }

    // Save to res
    res.user_data = QVariant::fromValue(std::move(mapped_images));

    if (!anki_db.open()) {
        return std::unexpected(QString("Failed to import anki DB: %1").arg(anki_db.lastError().text()));
    }

    // Select cards
    QSqlQuery cards_query(anki_db);
    QSqlQuery deck_name_query(anki_db);

    cards_query.prepare("SELECT notes.flds FROM cards JOIN notes ON notes.id = cards.nid");
    deck_name_query.prepare("SELECT name FROM decks");

    if (!cards_query.exec()) {
        return std::unexpected(QString("Failed to fetch cards from Anki DB: %1").arg(cards_query.lastError().text()));
    }

    if (!deck_name_query.exec()) {
        return std::unexpected(
            QString("Failed to fetch name from Anki DB: %1").arg(deck_name_query.lastError().text()));
    }

    deck_name_query.next(); // skip `Default`
    if (!deck_name_query.next()) {
        return std::unexpected(QString("Decks table is empty (Anki DB)"));
    }

    res.deck_name = deck_name_query.value("name").toString();
    res.deck_description = QString(); // TODO: implement reading description from Anki DB

    constexpr QChar FIELD_SEP{31}; // anki separator

    while (cards_query.next()) {
        const QString     flds = cards_query.value("flds").toString();
        const QStringList fields = flds.split(FIELD_SEP);

        if (fields.size() < 2) {
            continue; // broken note
        }

        QString front = fields.at(0).trimmed();
        QString back = fields.at(1).trimmed();

        res.cards.push_back(core::Card{.id = 0,      // not taken into account
                                       .deck_id = 0, // not taken into account
                                       .difficulty = 2.5f,
                                       .state = core::Card::State::New, // new
                                       .incorrect_streak = 0,
                                       .interval = 0,
                                       .time_limit = 0, // not taken into account
                                       .next_review = QDateTime::currentDateTime(),
                                       .created_at = QDateTime::currentDateTime(),
                                       .updated_at = QDateTime::currentDateTime(),
                                       .front = std::move(front),
                                       .back = std::move(back)});
    }

    return res;
}

// Public method
QString AnkiDeckImporter::format_name() const noexcept {
    return "anki";
}

// Private method
std::expected<void, QString> AnkiDeckImporter::unzip(const QString& zip_path, const QString& target_dir) {
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

// Private method
std::expected<void, QString> AnkiDeckImporter::decompress_zstd_file(const QString& src_path, const QString& dest_path) {
    QFile in(src_path);
    if (!in.open(QIODevice::ReadOnly))
        return std::unexpected(QString("Failed to open source: %1").arg(src_path));

    QFile out(dest_path);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return std::unexpected(QString("Failed to open destination: %1").arg(dest_path));

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if (!dctx)
        return std::unexpected("ZSTD_createDCtx failed");

    const size_t inBufSize = ZSTD_DStreamInSize();   // recommended input buffer size
    const size_t outBufSize = ZSTD_DStreamOutSize(); // recommended output buffer size

    std::vector<char> inBuf(inBufSize);
    std::vector<char> outBuf(outBufSize);

    ZSTD_inBuffer  input{inBuf.data(), 0, 0};
    ZSTD_outBuffer output{outBuf.data(), outBufSize, 0};

    std::optional<QString> error;

    while (!in.atEnd()) {
        qint64 bytesRead = in.read(inBuf.data(), static_cast<qint64>(inBufSize));
        if (bytesRead <= 0) {
            error = "Read error";
            break;
        }
        input.src = inBuf.data();
        input.size = static_cast<size_t>(bytesRead);
        input.pos = 0;

        while (input.pos < input.size) {
            output.pos = 0;
            size_t ret = ZSTD_decompressStream(dctx, &output, &input);
            if (ZSTD_isError(ret)) {
                error = QString("ZSTD_decompressStream failed: %1").arg(ZSTD_getErrorName(ret));
                break;
            }
            if (output.pos > 0) {
                qint64 wrote = out.write(outBuf.data(), static_cast<qint64>(output.pos));
                if (wrote != static_cast<qint64>(output.pos)) {
                    error = "Write error";
                    break;
                }
            }
        }
        if (error)
            break;
    }

    // flush final frames
    while (!error) {
        output.pos = 0;
        ZSTD_inBuffer emptyIn{nullptr, 0, 0};
        size_t        ret = ZSTD_decompressStream(dctx, &output, &emptyIn);
        if (ZSTD_isError(ret)) {
            error = QString("ZSTD final flush failed: %1").arg(ZSTD_getErrorName(ret));
            break;
        }
        if (output.pos == 0)
            break;
        qint64 wrote = out.write(outBuf.data(), static_cast<qint64>(output.pos));
        if (wrote != static_cast<qint64>(output.pos)) {
            error = "Write error";
            break;
        }
    }

    ZSTD_freeDCtx(dctx);
    out.close();
    in.close();

    if (error) {
        QFile::remove(dest_path);
        return std::unexpected(*error);
    }

    return {};
}

// Private method
QMap<QString, QString> AnkiDeckImporter::map_images(const QString& media_path) {
    QMap<QString, QString> result;

    QFile file(media_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "map_images: failed to open media file:" << media_path;
        return result;
    }

    // Read entire file (media files are typically not huge)
    const QByteArray raw = file.readAll();
    file.close();

    // Convert to QString using Latin1 to reliably preserve ASCII ranges inside arbitrary binary data.
    // Filenames like "paste-...jpg" are ASCII, so this is safe and avoids UTF-8 decoding quirks.
    const QString text = QString::fromLatin1(raw);

    // Regular expression to find image filenames like:
    //   paste-<hex-or-alphanum>.jpg
    //   paste-<...>.png
    // Adjust regex if you have other naming schemes.
    static const QRegularExpression re(R"(paste-[A-Za-z0-9_\-]+(?:\.[A-Za-z0-9]+))",
                                       QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator it = re.globalMatch(text);

    int index = 0;
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        if (!m.hasMatch())
            continue;

        const QString image_name = m.captured(0).trimmed();
        if (image_name.isEmpty())
            continue;

        // If duplicate image_name appears, we *do not* remap the key; keep first mapping.
        // This behaviour is intentional: QMap keys must be unique and typical Anki exports
        // have unique paste-... filenames. If duplicates are possible and undesirable,
        // consider using QMultiMap or mapping to QVector<QString>.
        if (!result.contains(image_name)) {
            result.insert(image_name, QString::number(index));
            ++index;
        } else {
            // Duplicate encountered: still increment index to keep archive counting consistent
            // (so the next unique name maps to the correct numeric archive).
            ++index;
        }
    }

    // If nothing matched, result will be empty.
    return result;
}

} // namespace io
