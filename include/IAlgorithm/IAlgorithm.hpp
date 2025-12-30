// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <Entities/Card.hpp> // for Card
#include <QString>           // for QString

namespace revise {

/**
 * @brief Algorithm result: updated card and boolean: `true` == should be returned to queue (relearn),
 * `false` == moved to trained list (schedule next review).
 */
struct AlgorithmResult {
    Card updated_card;
    bool requeue{false};
};

/**
 * @brief Interface for scheduling/repetition algorithms.
 *
 * Implementations must be pure functions: they must not perform I/O, only compute new card fields.
 */
class IAlgorithm {
  public:
    virtual ~IAlgorithm() = default;

    /**
     * @brief Process user's answer for a card.
     * @param old_card Card as it was before the answer (copyable).
     * @param user_difficulty User selection from UI: float in [0..5], where 5.0 means "I failed".
     * @return AlgorithmResult with updated card and whether to requeue it immediately.
     */
    virtual AlgorithmResult process_answer(const Card& old_card, float user_difficulty) noexcept = 0;
};

} // namespace revise
