// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "SqlEventRecorder.hpp" // for SqlEventRecorder header

namespace io {

SqlEventRecorder::SqlEventRecorder(std::shared_ptr<core::IEventStorage> event_storage) :
    core::IEventRecorder(event_storage) {}


std::expected<void, QString> SqlEventRecorder::record(const core::CardReviewEvent& event) {
    return m_event_storage->record(event);
}

} // namespace io
