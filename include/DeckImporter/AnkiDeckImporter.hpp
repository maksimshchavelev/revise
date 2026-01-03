// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A class that regulates the learning process. It determines which cards will be included in today's selection and
// manages the training process for the selected deck.

#pragma once

#include <IDeckImporter/IDeckImporter.hpp>         // for IDeckImporter

namespace revise {

class AnkiDeckImporter : public IDeckImporter {
public:
    std::expected<ImportResult, QString> import_from_file(const QString& path) override;

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
};

} // namespace revise
