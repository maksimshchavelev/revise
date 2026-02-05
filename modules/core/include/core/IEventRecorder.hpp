// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>  // for QString
#include <chrono>   // for std::chrono
#include <expected> // for std::expected

namespace core {

/**
 * @brief Card view event. Used with some implementation of the `IEventRecorder` event recorder.
 */
struct CardReviewEvent {
    int                       card_id;           ///< Local Card ID
    int                       global_card_id{0}; ///< Global card ID
    int                       deck_id;           ///< Local deck ID
    int                       global_deck_id{0}; ///< Global deck ID
    int                       current_streak;    ///< Current user streak
    std::chrono::milliseconds review_duration;   ///< How much time did the user spend on the answer?
};

/**
 * @brief Abstract event recorder class
 * @brief This class stores all events performed by the user. For example, this is useful for calculating ratings (the
 * user repeated the card - the rating increased).
 * @note Implement the necessary methods of this class yourself.
 */
class IEventRecorder {
  public:
    explicit IEventRecorder() = default;
    virtual ~IEventRecorder() = default;

    /**
     * @brief Save event
     * @param event Card review event `CardReviewEvent`
     * @return `std::expected<void, QString>` with `void` if success, otherwise `QString` with error description
     */
    virtual std::expected<void, QString> record(const CardReviewEvent& event) = 0;
};

} // namespace core
