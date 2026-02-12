// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "io/DeckMediaStorageFactory.hpp" // for header
#include "DeckMediaStorage.hpp"           // for DeckMediaStorage

namespace io {

std::unique_ptr<core::IDeckMediaStorage> create_deck_media_storage() {
    return std::make_unique<DeckMediaStorage>();
}

} // namespace io
