// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckImporter.hpp> // for core::IDeckImporter
#include <memory>                 // for std::unique_ptr

namespace io {

/**
 * @brief Used to specify the importer format (in the `io::create_deck_importer` function)
 */
enum class ImporterFormat {
    ANKI,  ///< Anki .apkg format
    REVISE ///< Revise .rpkg format
};

/**
 * @brief Creates a deck importer, returns a pointer to the interface
 * @param format The format that the importer will work with. For example, specifying `ImporterFormat::ANKI` will create
 * an importer that can work with the anki .apkg format.
 * @return `std::unique_ptr<core::IDeckImporter>`
 * @note If `format` is specified incorrectly, `nullptr` is returned.
 */
std::unique_ptr<core::IDeckImporter> create_deck_importer(const ImporterFormat format);

} // namespace io