// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckExporter.hpp> // for core::IDeckExporter
#include <memory>                 // for std::unique_ptr

namespace io {

/**
 * @brief Used to specify the exporter format (in the `io::create_deck_exporter` function)
 */
enum class ExporterFormat {
    REVISE ///< Revise .rpkg format
};

/**
 * @brief Creates a deck exporter, returns a pointer to the interface
 * @param format The format that the importer will work with. For example, specifying `ExporterFormat::REVISE` will
 * create an exporter that can work with the revise .rpkg format.
 * @return `std::unique_ptr<core::IDeckExporter>`
 * @note If `format` is specified incorrectly, `nullptr` is returned.
 */
std::unique_ptr<core::IDeckExporter> create_deck_exporter(const ExporterFormat format);

} // namespace io