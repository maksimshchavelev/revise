// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IEventRecorder.hpp> // for IEventRecorder

namespace io {

/**
 * @brief SQL database-based event recorder
 */
class SqlEventRecorder final : public core::IEventRecorder {
  public:
    explicit SqlEventRecorder(std::shared_ptr<core::IEventStorage> event_storage);

    /**
     * @see `core::IEventRecorder`
     */
    std::expected<void, QString> record(const core::CardReviewEvent& event) override;
};

} // namespace io
