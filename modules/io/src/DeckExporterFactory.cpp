// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/DeckExporterFactory.hpp" // for deck exporter factory
#include "ReviseDeckExporter.hpp"     // for ReviseDeckExporter

namespace io {

std::unique_ptr<core::IDeckExporter> create_deck_exporter(const ExporterFormat format) {
    switch (format) {
    case ExporterFormat::REVISE:
        return std::make_unique<ReviseDeckExporter>();
    default:
        return nullptr;
    }
}

} // namespace io