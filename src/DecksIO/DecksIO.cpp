// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Support class for importing and exporting decks

#include <DecksIO/DecksIO.hpp>             // for DecksIO
#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter
#include <QDir>                            // for QDir
#include <QSqlError>                       // for QSqlError
#include <QSqlQuery>                       // for QSqlQuery
#include <QStandardPaths>                  // for QStandardPaths
#include <QUuid>                           // for QUuid
#include <QtConcurrent>                    // for QtConcurrent
#include <quazip.h>                        // for QuaZip
#include <quazipfile.h>                    // for QuaZipFile
#include <zstd.h>                          // for ZSTD


namespace revise {

// Public method
revise::DecksIO::DecksIO(/*Database& db, */ QObject* parent) : QObject(parent) {}


// Public method
void DecksIO::import_anki_deck_async(const QString& path) {
    if (m_in_progress) {
        ErrorReporter::instance()->report("Import already in progress",
                                          "DecksIO::import_anki_deck_async: other import/export already in progress",
                                          "DecksIO::import_anki_deck_async()");
        return;
    }

    m_in_progress = true;
    emit in_progress_changed();

    [[maybe_unused]] QFuture<void> future = QtConcurrent::run([this, path]() {
        import_anki_deck(path);

        QMetaObject::invokeMethod(
            this,
            [this]() {
                m_in_progress = false;
                emit in_progress_changed();
            },
            Qt::QueuedConnection);
    });
}


// Private method
void DecksIO::import_anki_deck(const QString& path) {
    quint32 imported_decks{0}; // counter of imported decks

    {
        Database db("import_connection", this);
        qDebug() << "DecksIO::import_anki_deck() creating directories and unzipping .apkg...";

        QString base_temp_dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        QString import_dir = base_temp_dir + "/anki_import_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
        QString db_path = import_dir + "/collection";

        QDir().mkdir(import_dir); // make import directory

        // Unzip .apkg
        if (auto res = unzip(path, import_dir); !res.has_value()) {
            ErrorReporter::instance()->report("Failed to unzip .apkg", res.error(), "DecksIO::import_anki_deck()");
            return;
        }

        // Unzip collection.anki21b
        if (auto res = decompress_zstd_file(import_dir + "/collection.anki21b", db_path); !res.has_value()) {
            ErrorReporter::instance()->report(
                "Failed to decompress zstd file", res.error(), "DecksIO::import_anki_deck()");
            return;
        }

        qDebug() << "DecksIO::import_anki_deck(): Unzipped" << path << "working with db" + db_path;

        if (!QFile::exists(db_path)) {
            ErrorReporter::instance()->report("DB not found", "DB path: " + db_path, "DecksIO::import_anki_deck()");
            return;
        }

        QSqlDatabase anki_db = QSqlDatabase::addDatabase("QSQLITE", "anki_import");
        anki_db.setDatabaseName(db_path);

        if (!anki_db.open()) {
            ErrorReporter::instance()->report(
                "Failed to open Anki DB", anki_db.lastError().text(), "DecksIO::import_anki_deck()");
            return;
        }

        qDebug() << "DecksIO::import_anki_deck(): DB is opened";

        // Select cards
        {
            QSqlQuery cards_query(anki_db);
            QSqlQuery deck_name_query(anki_db);

            cards_query.prepare("SELECT notes.flds FROM cards JOIN notes ON notes.id = cards.nid");
            deck_name_query.prepare("SELECT name FROM decks");

            if (!cards_query.exec()) {
                ErrorReporter::instance()->report("Failed to fetch cards from Anki DB",
                                                  cards_query.lastError().text(),
                                                  "DecksIO::import_anki_deck()");
                return;
            }

            if (!deck_name_query.exec()) {
                ErrorReporter::instance()->report("Failed to fetch deck name from Anki DB",
                                                  deck_name_query.lastError().text(),
                                                  "DecksIO::import_anki_deck()");
                return;
            }

            deck_name_query.next(); // skip `Default`
            if (!deck_name_query.next()) {
                ErrorReporter::instance()->report("Decks table is empty", {}, "DecksIO::import_anki_deck()");
                return;
            }

            auto deck_id = db.push_deck(deck_name_query.value("name").toString()); // Create new deck and get deck id

            if (!deck_id.has_value()) {
                ErrorReporter::instance()->report(
                    "Failed to get deck id", deck_id.error(), "DecksIO::import_anki_deck()");
                return;
            }

            constexpr QChar FIELD_SEP{31}; // anki separator
            db.begin_bulk_updates();

            while (cards_query.next()) {
                const QString     flds = cards_query.value("flds").toString();
                const QStringList fields = flds.split(FIELD_SEP);

                if (fields.size() < 2) {
                    continue; // broken note
                }

                QString front = fields.at(0).trimmed();
                QString back = fields.at(1).trimmed();

                if (auto res = db.push_card(deck_id.value(), front, back); !res.has_value()) {
                    db.end_bulk_updates();
                    ErrorReporter::instance()->report(
                        "Failed to push card to database", res.error(), "DecksIO::import_anki_deck()");
                    return;
                } else {
                    ++imported_decks;
                }
            }
        }

        // qWarning() << "END BULK!";
        db.end_bulk_updates();

        qDebug() << "DecksIO::import_anki_deck(): Imported. Clearing resources";

        // Clear resources
        anki_db.close();
        QSqlDatabase::removeDatabase("anki_import");
        QDir(import_dir).removeRecursively();
    }

    emit import_finished(imported_decks);

    return;
}


// Public method
bool DecksIO::in_progress() const noexcept {
    return m_in_progress;
}


// Private method
std::expected<void, QString> DecksIO::unzip(const QString& zip_path, const QString& target_dir) {
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
std::expected<void, QString> DecksIO::decompress_zstd_file(const QString& src_path, const QString& dest_path) {
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

} // namespace revise
