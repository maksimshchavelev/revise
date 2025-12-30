// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Support class for importing and exporting decks

#pragma once

#include <Database/Database.hpp> // for Database

namespace revise {

class DecksIO : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(bool inProgress READ in_progress NOTIFY in_progress_changed)

    DecksIO(/*Database& db,*/ QObject* parent = nullptr);

    /**
     * @brief Imports Anki deck
     * @param path Path to decl
     * @note Watch for the `error_occured` signal. If an error occurs, it will be emitted.
     * @note Keep an eye on the `inProgress` property, which shows whether the import is in progress.
     */
    Q_INVOKABLE void import_anki_deck_async(const QString& path);

    /**
     * @brief Is import/export in progress?
     * @return `true` if in progress
     */
    bool in_progress() const noexcept;

  signals:
    void in_progress_changed();
    void error_occured(const QString& error);
    void import_finished(int cards_imported);

  private:
    //Database m_db;
    bool     m_in_progress{false};

    /**
     * @brief Import anki deck
     * @param path Path to anki .apkg
     * @warning Run via QtConcurrent, or use `import_anki_deck_async`
     * @note Watch for the `error_occured` signal. If an error occurs, it will be emitted.
     */
    void import_anki_deck(const QString& path);

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
