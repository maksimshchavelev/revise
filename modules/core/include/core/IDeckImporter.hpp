// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <core/structs/Card.hpp> // for core::Card
#include <expected>              // for std::expected

namespace core {

/**
 * @brief Result of an import operation.
 *
 * Contains logical deck name and the list of cards parsed from the source file.
 */
struct ImportResult {
    QString       deck_name;        ///< Deck name
    QString       deck_description; ///< Deck description
    int           time_limit;       ///< Time limit
    int           new_limit;        ///< Limit of new cards
    int           review_limit;     ///< Limit of review cards
    int           incorrect_limit;  ///< Limit of incorrect cards
    QVector<Card> cards;            ///< parsed cards (front/back and meta). Use repository Card DTO.
    QVariant      user_data;        ///< Custom data. Depends on the specific implementation of the importer.
};

/**
 * @brief Interface for importing decks from external formats (apkg, JSON, csv, ...).
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
    /// @brief An error caused by the importer
    struct Error {
        /// @brief Kind of the error
        enum Kind {
            InternalError,    ///< Internal Error
            UnknownFormat,    ///< Unknown input file format
            FileCorrupted,    ///< The input file is corrupted or does not contain the necessary data
            ExtractionFailed, ///< Failed to extract data from input file
            FailedToOpenFile, ///< Unable to open the file for import
            NotSupported,     ///< Method of import is not supported
            OutdatedFormat,   ///< The data format is outdated (for example, the deck was exported using a version of the app that
                              ///< is too old)
        };

        Kind    kind;    ///< Kind of error
        QString message; ///< Optional message of error

        /// @brief Make `InternalError` error with message
        static Error internal(QString message) {
            return Error{InternalError, std::move(message)};
        }

        /// @brief Make `UnknownFormat` error with message
        static Error unknown_format(QString message) {
            return Error{UnknownFormat, std::move(message)};
        }

        /// @brief Make `FileCorrupted` error with message
        static Error file_corrupted(QString message) {
            return Error{FileCorrupted, std::move(message)};
        }

        /// @brief Make `ExtractionFailed` error with message
        static Error extraction_failed(QString message) {
            return Error{ExtractionFailed, std::move(message)};
        }

        /// @brief Make `FailedToOpenFile` error with message
        static Error failed_to_open_file(QString message) {
            return Error{FailedToOpenFile, std::move(message)};
        }

        /// @brief Make `NotSupported` error with message
        static Error not_supported(QString message) {
            return Error{NotSupported, std::move(message)};
        }

        /// @brief Make `OutdatedFormat` error with message
        static Error outdated_format(QString message) {
            return Error{OutdatedFormat, std::move(message)};
        }
    };

    using ErrorKind = Error::Kind;
    template <typename T = void> using Result = std::expected<T, Error>;

    IDeckImporter() = default;
    virtual ~IDeckImporter() = default;

    /**
     * @brief Import deck from the provided file path.
     * @param path Path to archive/file to import
     * @return `Result` containing ImportResult on success or `Error` error message on failure.
     *
     * Notes for implementors:
     * - The method must not perform any UI interaction.
     * - It must not persist parsed cards into the application's DB. Return parsed cards
     *   and let higher level (decks storage / service) decide how/where to store them.
     * - Because this runs in a worker thread, make sure to avoid touching QObjects that live in the main thread.
     *
     * @note You do not need to implement this method; if you do not, a `NotSupported` error will be returned when it is called.
     */
    [[nodiscard]] virtual Result<ImportResult> import_from_file(const QString& path) const {
        return std::unexpected(Error::not_supported("File format not supported"));
    }

    /**
     * @brief Import deck from the provided text.
     * @param text The text from which you need to extract the deck and the cards
     * @return `Result` containing ImportResult on success or `Error` error message on failure.
     *
     * Notes for implementors:
     * - The method must not perform any UI interaction.
     * - It must not persist parsed cards into the application's DB. Return parsed cards
     *   and let higher level (decks storage / service) decide how/where to store them.
     * - Because this runs in a worker thread, make sure to avoid touching QObjects that live in the main thread.
     *
     * @note You do not need to implement this method; if you do not, a `NotSupported` error will be returned when it is called.
     */
    [[nodiscard]] virtual Result<ImportResult> import_from_text(const QString& text) const {
        return std::unexpected(Error::not_supported("Text format not supported"));
    }

    /**
     * @brief Short identifier of importer (e.g. "anki", "csv", "json").
     * @return short name
     */
    [[nodiscard]] virtual QString format_name() const = 0;
};

} // namespace core
