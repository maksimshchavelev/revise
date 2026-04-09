// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Card.hpp" // for Card
#include <expected> // for std::expected
#include <optional> // for std::optional

namespace core {

/**
 * @brief The engine that drives the training process
 */
class IStudyEngine {
  public:
    virtual ~IStudyEngine() = default;

    /**
     * @brief Current training session parameters
     */
    struct SessionState {
        std::optional<Card> current_card;   ///< Current card
        int                 time_limit;     ///< Time limit
        float               time_remaining; ///< Time remaining
        bool                flipped;        ///< Is card flipped
    };

    /**
     * @brief Result of response to card
     */
    struct StepResult {
        bool                finished;  ///< Is this the last card?
        std::optional<Card> next_card; ///< Next card. May not contain a value if `finished` is `true`.
    };

    /**
     * @brief Information about deck that engine can compute (counts and limits).
     */
    struct StudyInfo {
        int new_cards{0};
        int consolidate_cards{0};
        int incorrect_cards{0};
        int time_limit{0};
    };

    /**
     * @brief Start learning the deck
     * @param deck_id ID of the deck to be learned
     * @return `std::expected<void, QString>` with `void` in case of success or with an error description in case of
     * failure
     */
    virtual std::expected<void, QString> start(int deck_id) = 0;

    /**
     * @brief Reply to card
     * @param difficulty How the user rates the complexity of the card
     * @return `std::expected<StepResult, QString>` with `StepResult` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<StepResult, QString> answer(float difficulty) = 0;

    /**
     * @brief Flip current card
     * @return `std::expected<void, QString>` with `void` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<void, QString> flip() = 0;

    /**
     * @brief Get the current state of the training session
     * @return `SessionState&`
     */
    virtual SessionState& state() = 0;

    /**
     * @brief Reloads session state
     * @return `std::expected<void, QString>` with `void` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<void, QString> reload_state() = 0;

    /**
     * @brief Abort current session
     * @return `std::expected<void, QString>` with `void` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<void, QString> abort() = 0;

    /**
     * @brief Save trained cards
     * @return `std::expected<void, QString>` with `void` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<void, QString> save_trained() = 0;

    /**
     * @brief Compute study info for a deck (counts limited by deck's limits).
     *
     * @param deck_id Deck id
     * @return `std::expected<StudyInfo, QString>` with `StudyInfo` in case of success or with an error description in
     * case of failure
     */
    virtual std::expected<StudyInfo, QString> get_study_info(int deck_id) const = 0;
};

} // namespace core
