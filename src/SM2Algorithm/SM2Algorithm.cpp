// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <SM2Algorithm/SM2Algorithm.hpp> // for SM2Algorithm

namespace revise {

AlgorithmResult SM2Algorithm::process_answer(const Card& old_card, float user_difficulty) noexcept {
    AlgorithmResult r;
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

        // Smoothly increase card difficulty (EMA towards 5.0)
        r.updated_card.difficulty =
            clamp_difficulty(r.updated_card.difficulty + ALPHA_FAILURE * (5.0f - r.updated_card.difficulty));

        // Return the card to the end of the active queue
        r.requeue = true;
    } else {
        // The answer is considered successful
        if (r.updated_card.incorrect_streak > 0 && r.updated_card.difficulty > 2.5) {
            r.updated_card.state = 2; // still difficult
        } else {
            r.updated_card.state = 1;
            r.updated_card.incorrect_streak = 0;
        }

        float           x = 1.0f - (r.updated_card.difficulty / 5.0f); // 0 = hard, 1 = easy
        constexpr float maxDays = 10.0f;
        constexpr float k = 2.2f; // curve steepness

        float intervalDays = maxDays * (std::exp(k * x) - 1.0f) / (std::exp(k) - 1.0f);

        intervalDays = std::clamp(intervalDays, 0.5f, maxDays);

        r.updated_card.interval = static_cast<int>(std::round(intervalDays));
        r.updated_card.next_review = QDateTime::currentDateTime().addDays(r.updated_card.interval);
        // Normalize to midnight
        r.updated_card.next_review.setTime(QTime(0, 0, 0));

        // Smoothly adapt difficulty towards the user's assessment
        r.updated_card.difficulty = clamp_difficulty(r.updated_card.difficulty + ALPHA_SUCCESS *
                                                                 (user_difficulty - r.updated_card.difficulty));
    }

    return r;
}

} // namespace revise
