// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IEventRecorder.hpp> // for IEventRecorder

namespace io {

/**
 * @brief The function creates an event recorder.
 * @param event_storage Pointer to `core::IEventStorage` interface
 * @return Pointer to `core::IEventRecorder` interface
 */
std::unique_ptr<core::IEventRecorder> create_event_recorder(std::shared_ptr<core::IEventStorage> event_storage);

} // namespace io
