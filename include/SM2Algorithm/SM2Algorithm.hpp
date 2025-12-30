// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <IAlgorithm/IAlgorithm.hpp> // for IAlgorithm

namespace revise {

/**
 * @brief SM-2 algorithm implementation adapted for our Card DTO.
 *
 * Notes:
 * - Card.difficulty is used to store the "EF" (easiness factor)-like value in a compact form.
 *   The mapping is kept inside implementation (we read difficulty -> ef and write ef back into difficulty).
 * - Repetition count is NOT stored in DB here; StudyService maintains an in-memory repetitions map.
 * - user_difficulty is converted to "quality" as SM-2 expects (0..5).
 */
class SM2Algorithm : public IAlgorithm {
public:
    SM2Algorithm() = default;
    ~SM2Algorithm() override = default;

    AlgorithmResult process_answer(const Card& old_card, float user_difficulty) noexcept override;

private:
    // Helpers
    static float difficulty_to_ef(float difficulty) noexcept;
    static float ef_to_difficulty(float ef) noexcept;
    static int quality_from_user_difficulty(float user_difficulty) noexcept;
};

} // namespace revise
