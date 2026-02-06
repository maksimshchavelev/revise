// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "IEventStorage.hpp" // for IEventStorage

namespace core {

/**
 * @brief Abstract event recorder class
 * @brief This class stores all events performed by the user. For example, this is useful for calculating ratings (the
 * user repeated the card - the rating increased).
 * @note Implement the necessary methods of this class yourself.
 */
class IEventRecorder {
  public:
    /**
     * @brief IEventRecorder constructor
     * @param event_storage Event storage interface
     */
    explicit IEventRecorder(std::shared_ptr<IEventStorage> event_storage);

    virtual ~IEventRecorder() = default;

    /**
     * @brief Save event
     * @param event Card review event `CardReviewEvent`
     * @return `std::expected<void, QString>` with `void` if success, otherwise `QString` with error description
     */
    virtual std::expected<void, QString> record(const CardReviewEvent& event) = 0;

  protected:
    std::shared_ptr<IEventStorage> m_event_storage; ///< Event storage
};

} // namespace core
