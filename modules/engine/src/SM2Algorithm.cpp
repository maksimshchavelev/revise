// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "SM2Algorithm.hpp" // for header

namespace engine {

SM2Algorithm::SM2Algorithm(const core::AlgorithmSettings& settings) : m_settings(settings) {}

core::AlgorithmResult SM2Algorithm::process_answer(const core::Card& old_card, float user_difficulty) noexcept {
    core::AlgorithmResult r;
    r.updated_card = old_card; // copy

    // Clamp helper to keep difficulty within [0; 5]
    auto clamp_difficulty = [](float value) { return std::clamp(value, 0.0f, 5.0f); };

    constexpr float FAILURE_THRESHOLD = 5.0f;
    constexpr float ALPHA_SUCCESS = 0.15f;
    constexpr float ALPHA_FAILURE = 0.30f;

    if (user_difficulty >= FAILURE_THRESHOLD) {
        // The user made a mistake
        r.updated_card.state = 2; // incorrect
        r.updated_card.incorrect_streak += 1;

        r.updated_card.difficulty =
            clamp_difficulty(r.updated_card.difficulty + ALPHA_FAILURE * (5.0f - r.updated_card.difficulty));

        r.requeue = true;
    } else {
        // The answer is considered successful
        if (r.updated_card.incorrect_streak > 0 && r.updated_card.difficulty > 2.5) {
            r.updated_card.state = 2; // still difficult
        } else {
            r.updated_card.state = 1;
            r.updated_card.incorrect_streak = 0;
        }

        float       x = 1.0f - (r.updated_card.difficulty / 5.0f); // 0 = hard, 1 = easy
        const int   max_days = m_settings.max_interval();
        const float k = m_settings.learning_rate(); // curve steepness

        int interval_days = max_days * (std::exp(k * x) - 1.0f) / (std::exp(k) - 1.0f);

        interval_days = std::clamp(interval_days, 1, max_days);

        r.updated_card.interval = static_cast<int>(std::round(interval_days));
        r.updated_card.next_review = QDateTime::currentDateTime().addDays(r.updated_card.interval);
        r.updated_card.next_review.setTime(QTime(0, 0, 0));

        // Smoothly adapt difficulty towards the user's assessment
        r.updated_card.difficulty =
            clamp_difficulty(r.updated_card.difficulty + ALPHA_SUCCESS * (user_difficulty - r.updated_card.difficulty));
    }

    return r;
}

} // namespace engine
