// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckExporter.hpp> // for core::IDeckExporter

namespace io {

/**
 * @brief Class to export to revise (*rpkg) format
 */
class ReviseDeckExporter : public core::IDeckExporter {
  public:
    std::expected<void, QString> export_to_file(const core::ExportData& data, const QString& path) override;
    QString                      format_name() const noexcept override;

  private:
    /**
     * @brief Compresses the contents of a directory into a ZIP archive.
     *
     * The function recursively traverses the given directory and adds all files
     * and subdirectories to the ZIP archive. The root directory itself is NOT
     * included — only its contents. When the archive is opened, files will be
     * visible directly, not nested under the source directory name.
     *
     * @param source_dir_path Absolute path to the directory to compress.
     * @param zip_file_path Absolute path to the resulting ZIP archive.
     *
     * @return std::expected<void, QString>
     *         Returns an error description on failure, or void on success.
     */
    std::expected<void, QString> compress_directory_to_zip(const QString& source_dir_path,
                                                           const QString& zip_file_path);

    /**
     * @brief Strips directory paths from img src attributes in HTML.
     *
     * The function finds all <img> tags in the input HTML string and modifies
     * their src attributes by removing the directory path, leaving only the
     * filename and extension.
     *
     * Example:
     *   <img src="file:///storage/emulated/0/media/image.jpg">
     * becomes:
     *   <img src="image.jpg">
     *
     * @param html Input HTML string.
     * @return Updated HTML string with stripped img src paths.
     */
    QString strip_img_src_paths(const QString& html);
};

} // namespace io
