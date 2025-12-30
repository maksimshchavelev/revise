// Copyright 2025 Maksim Shchavelev
// Interface for deck importers (Anki, custom formats, etc.)

#pragma once

#include <IDeckRepository/IDeckRepository.hpp> // for IDeckRepository
#include <QString>                             // for QString
#include <QVector>                             // for QVector
#include <expected>                            // for std::expected

namespace revise {

/**
 * @brief Result of an import operation.
 *
 * Contains logical deck name and the list of cards parsed from the source file.
 */
struct ImportResult {
    QString       deck_name; ///< human-readable deck name
    QVector<Card> cards;     ///< parsed cards (front/back and meta). Use repository Card DTO.
};

/**
 * @brief Interface for importing decks from external formats (apkg, json, csv, ...).
 *
 * Implementations MUST be thread-safe in the sense that they can be executed from a worker thread.
 * The method import_from_file() is synchronous: it parses the file and returns the parsed result
 * or an error description. DecksIO or the caller is responsible for running this synchronously
 * inside a thread pool if async behavior is required.
 *
 * Do NOT make this interface a QObject — keep it plain and easy to mock in unit tests.
 */
class IDeckImporter {
  public:
    IDeckImporter() = default;
    virtual ~IDeckImporter() = default;

    /**
     * @brief Import deck from the provided file path.
     * @param path Path to archive/file to import (for Anki - .apkg)
     * @return std::expected containing ImportResult on success or QString error message on failure.
     *
     * Notes for implementors:
     * - The method must not perform any UI interaction.
     * - It must not persist parsed cards into the application's DB. Return parsed cards
     *   and let higher level (DecksIO / service) decide how/where to store them.
     * - Because this runs in a worker thread, make sure to avoid touching QObjects that live in the main thread.
     */
    virtual std::expected<ImportResult, QString> import_from_file(const QString& path) = 0;

    /**
     * @brief Short identifier of importer (e.g. "anki", "csv", "json").
     * @return short name
     */
    virtual QString format_name() const noexcept = 0;
};

} // namespace revise
