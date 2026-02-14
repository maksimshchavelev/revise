// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Card.hpp"       // for Card
#include "Notifiable.hpp" // for Notifiable
#include <expected>       // for std::expected
#include <optional>       // for std::optional

namespace core {

/**
 * @brief Service that manages the learning process
 */
class IStudyService : public Notifiable {
  public:
    EVENT(training_started)                          ///< Training started
    EVENT(training_finished)                         ///< Training finished
    EVENT(training_aborted)                          ///< Training aborted
    EVENT(card_changed, QString front; QString back) ///< Card changed (for example, next card)
    EVENT(remaining_time_changed, float remaining)   ///< Remaining time changed
    EVENT(flipped_changed, bool flipped)             ///< Card flip changed
    EVENT(error, QString message)                    ///< Error occurred

    virtual ~IStudyService() = default;

    /**
     * @brief Start learning the deck
     * @param deck_id ID of the deck to be learned
     * @note Emits the following events:
     * - `training_started` upon a **successful** start to training
     * - `error` in case of error (with error description)
     * - `remaining_time_changed` when the time remaining for the answer changes (a separate timer starts counting down)
     */
    virtual void start(int deck_id) = 0;

    /**
     * @brief Reply to card
     * @param difficulty How the user rates the complexity of the card
     * @note Emits the following events:
     * - `card_changed` if there are still cards to study and the next card has been successfully loaded
     * - `flipped_changed` when there are still cards left to study and the next card has been successfully loaded (this
     * event should be interpreted as turning the card over from the back to the front)
     * - `training_finished` when there are no more cards left to study and the training is complete
     * - `error` in case of error (with error description)
     */
    virtual void answer(float difficulty) = 0;

    /**
     * @brief Flip current card
     * @note Emits the following events:
     * - `error` in case of error (with error description)
     * - `flipped_changed` in the event that the card is successfully turned over
     */
    virtual void flip() = 0;

    /**
     * @brief Abort current session
     * @note Emits the following events:
     * - `training_aborted` when training is interrupted **successfully**
     * - `error` in case of error (with error description)
     */
    virtual void abort() = 0;

    /**
     * @brief Get current card
     * @return `const std::optional<core::Card>&` with `core::Card&` if success
     */
    virtual const std::optional<core::Card>& current_card() const = 0;

    /**
     * @brief Get total time limit for the current card
     * @return Time limit
     */
    virtual int time_limit() const = 0;

    /**
     * @brief Get remaining time for the current card
     * @return Remaining time
     */
    virtual float time_remaining() const = 0;

    /**
     * @brief Is card flipped
     * @return `true` if card flipped
     */
    virtual bool flipped() const = 0;
};

} // namespace core
