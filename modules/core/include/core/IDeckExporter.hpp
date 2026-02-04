// Copyright 2025 Maksim Shchavelev

#pragma once

#include "Card.hpp" // for Card
#include "Deck.hpp" // for Deck
#include <QString>  // for QString
#include <QVector>  // for QVector
#include <expected> // for std::expected

namespace core {

/**
 * @brief Data for export operation
 */
struct ExportData {
    Deck          deck;            ///< Deck
    QString       media_directory; ///< Media directory path. Empty by default
    QVector<Card> cards;           ///< Cards
};

/**
 * @brief Interface for exporting decks
 *
 * Implementations MUST be thread-safe in the sense that they can be executed from a worker thread.
 * The method export_to_file() is synchronous: it parses the file and returns the parsed result
 * or an error description. DecksIO or the caller is responsible for running this synchronously
 * inside a thread pool if async behavior is required.
 *
 * Do NOT make this interface a QObject — keep it plain and easy to mock in unit tests.
 */
class IDeckExporter {
  public:
    IDeckExporter() = default;
    virtual ~IDeckExporter() = default;

    /**
     * @brief Import deck from the provided file path.
     * @param ExportData export dataa
     * @param path Path to export
     * @return std::expected containing ImportResult on success or QString error message on failure.
     *
     * Notes for implementors:
     * - The method must not perform any UI interaction.
     * - It must not persist parsed cards into the application's DB. Return parsed cards
     *   and let higher level (DecksIO / service) decide how/where to store them.
     * - Because this runs in a worker thread, make sure to avoid touching QObjects that live in the main thread.
     */
    virtual std::expected<void, QString> export_to_file(const ExportData& data, const QString& path) = 0;

    /**
     * @brief Short identifier of importer (e.g. "anki", "csv", "json").
     * @return short name
     */
    virtual QString format_name() const noexcept = 0;
};

} // namespace core
