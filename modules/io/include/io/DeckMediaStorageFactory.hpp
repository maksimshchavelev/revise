// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckMediaStorage.hpp> // for core::IDeckMediaStorage

namespace io {

/**
 * @brief The function creates a deck media storage
 * @return Pointer to `core::IDeckMediaStorage` interface
 */
std::unique_ptr<core::IDeckMediaStorage> create_deck_media_storage();

} // namespace io
