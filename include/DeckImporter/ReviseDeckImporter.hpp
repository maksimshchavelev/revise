// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <IDeckImporter/IDeckImporter.hpp> // for IDeckImporter

namespace revise {

class ReviseDeckImporter : public IDeckImporter {
  public:
    /**
     * @brief Import revise deck
     * @param path Path to revise deck
     * @return std::expected<ImportResult, QString>
     * @note `ImportResult` contains `QMap<QString, QByteArray>`, where QString is image name,
     * QByteArray - binary data of the image
     */
    std::expected<ImportResult, QString> import_from_file(const QString& path) override;

    /**
     * @brief Format of deck
     * @return `revise` string
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
};

} // namespace revise
