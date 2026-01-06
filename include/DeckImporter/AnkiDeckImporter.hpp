// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A class that regulates the learning process. It determines which cards will be included in today's selection and
// manages the training process for the selected deck.

#pragma once

#include <IDeckImporter/IDeckImporter.hpp>         // for IDeckImporter

namespace revise {

class AnkiDeckImporter : public IDeckImporter {
public:
    /**
     * @brief Import anki deck
     * @param path Path to anki deck
     * @return std::expected<ImportResult, QString>
     * @note `ImportResult` contains `QMap<QString, QByteArray>`, where QString is image name,
     * QByteArray - binary data of the image
     */
    std::expected<ImportResult, QString> import_from_file(const QString& path) override;

    /**
     * @brief Format of deck
     * @return `anki` string
     */
    QString format_name() const noexcept override;

private:
    /**
     * @brief Unzip archive
     * @param zip_path Path to zip
     * @param target_dir Which directory should the archive be extracted to?
     * @return `std::expected<void, QString>` with `void` if success, otherwise with error description
     */
    std::expected<void, QString> unzip(const QString& zip_path, const QString& target_dir);

    /**
     * @brief Decompress entire zstd file into `dest_path`
     * @return std::expected<void, QString> error or empty on success
     */
    std::expected<void, QString> decompress_zstd_file(const QString& src_path, const QString& dest_path);

    /**
    * @brief Parse Anki 'media' binary file and map embedded image names to archive indices.
    *
    * The media file contains repeated records with ASCII image names (e.g. "paste-06b6f3d8ed9719... .jpg")
    * and binary payloads. In the .apkg layout the actual image binary data is stored in separate
    * numbered archives (0..N-1). This function does NOT extract or touch the archives — it only
    * reads `media_path`, finds image names in the order they appear and returns a mapping:
    *
    * image name -> archive name (stringified index, e.g. "0", "1", ...)
    *
    * The returned archive names correspond to the order of images in the media file (first found name
    * -> "0", second -> "1", ...). The caller is responsible for locating the actual archive files
    * (usually neighboring files named "0", "1", ... inside the unpacked .apkg directory).
    *
    * @param media_path path to the binary 'media' file inside unpacked .apkg
    * @return QMap where key is the original image name (as seen in <img src="...">) and value is the
    *         corresponding archive name (index as QString). Empty map on error/no matches.
    */
    QMap<QString /* image name */, QString /* archive name */> map_images(const QString& media_path);

};

} // namespace revise
