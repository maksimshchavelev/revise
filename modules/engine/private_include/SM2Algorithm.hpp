// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IAlgorithm.hpp>                 // for core::IAlgorithm
#include <core/settings/AlgorithmSettings.hpp> // for AlgorithmSettings

namespace engine {

/**
 * @brief SM-2 algorithm implementation adapted for our Card DTO.
 *
 * Notes:
 * - Card.difficulty is used to store the "EF" (easiness factor)-like value in a compact form.
 *   The mapping is kept inside implementation (we read difficulty -> ef and write ef back into difficulty).
 * - Repetition count is NOT stored in DB here; StudyService maintains an in-memory repetitions map.
 * - user_difficulty is converted to "quality" as SM-2 expects (0..5).
 */
class SM2Algorithm final : public core::IAlgorithm {
  public:
    SM2Algorithm(const core::AlgorithmSettings& settings);
    ~SM2Algorithm() override = default;

    core::AlgorithmResult process_answer(const core::Card& old_card, float user_difficulty) noexcept override;

  private:
    const core::AlgorithmSettings& m_settings;
};

} // namespace engine
