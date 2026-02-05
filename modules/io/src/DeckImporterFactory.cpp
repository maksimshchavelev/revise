// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "io/DeckImporterFactory.hpp" // for deck importer factory
#include "AnkiDeckImporter.hpp"       // for AnkiDeckImporter
#include "ReviseDeckImporter.hpp"     // for ReviseDeckImporter

namespace io {

std::unique_ptr<core::IDeckImporter> create_deck_importer(const ImporterFormat format) {
    switch (format) {
    case ImporterFormat::ANKI:
        return std::make_unique<AnkiDeckImporter>();
    case ImporterFormat::REVISE:
        return std::make_unique<ReviseDeckImporter>();
    default:
        return nullptr;
    }
}

} // namespace io