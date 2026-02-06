// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "io/EventRecorderFactory.hpp" // for header
#include "SqlEventRecorder.hpp"        // for SqlEventRecorder

namespace io {

std::unique_ptr<core::IEventRecorder> create_event_recorder(std::shared_ptr<core::IEventStorage> event_storage) {
    return std::make_unique<SqlEventRecorder>(event_storage);
}

} // namespace io
