// Copyright (C) 2025-2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <core/IDeckImporter.hpp> // for core::IDeckImporter

namespace io {

/**
 * @brief Imports the Revise deck. Only reads it and returns the result. The calling code must place the decks and cards
 * in the database itself.
 */
class ReviseDeckImporter : public core::IDeckImporter {
  public:
    /**
     * @brief Import revise deck
     * @param path Path to revise deck
     * @return std::expected<ImportResult, QString>
     * @note `ImportResult` contains `QMap<QString, QByteArray>`, where QString is image name,
     * QByteArray - binary data of the image
     */
    [[nodiscard]] Result<core::ImportResult> import_from_file(const QString& path) const override;

    /**
     * @brief Format of deck
     * @return `revise` string
     */
    [[nodiscard]] QString format_name() const override;

  private:
    /**
     * @brief Unzip archive
     * @param zip_path Path to zip
     * @param target_dir Which directory should the archive be extracted to?
     * @return `std::expected<void, QString>` with `void` if success, otherwise with error description
     */
    static std::expected<void, QString> unzip(const QString& zip_path, const QString& target_dir);
};

} // namespace io
