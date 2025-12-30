// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <SM2Algorithm/SM2Algorithm.hpp> // for SM2Algorithm

namespace revise {

/**
 * Convert stored difficulty (0..5) to EF (easiness factor) used by SM-2.
 * We map difficulty 0..5 -> ef range [1.3 .. 2.5] (higher EF = easier).
 */
float SM2Algorithm::difficulty_to_ef(float difficulty) noexcept {
    // clamp 0..5
    float d = std::clamp(difficulty, 0.0f, 5.0f);
    // invert: difficulty 0 -> easiest -> max EF
    // choose linear mapping: ef = 1.3 + (5 - d) * ((2.5f - 1.3f) / 5)
    constexpr float ef_min = 1.3f;
    constexpr float ef_max = 2.5f;
    return ef_min + (5.0f - d) * ((ef_max - ef_min) / 5.0f);
}

/**
 * Convert EF back to difficulty (0..5) — inverse of above.
 */
float SM2Algorithm::ef_to_difficulty(float ef) noexcept {
    constexpr float ef_min = 1.3f;
    constexpr float ef_max = 2.5f;
    float           clamped = std::clamp(ef, ef_min, ef_max);
    // invert linear mapping
    float d = 5.0f - (clamped - ef_min) * (5.0f / (ef_max - ef_min));
    return std::clamp(d, 0.0f, 5.0f);
}

/**
 * Convert user difficulty (0..5, 5==wrong) to SM-2 quality (0..5, 5==perfect).
 */
int SM2Algorithm::quality_from_user_difficulty(float user_difficulty) noexcept {
    if (user_difficulty >= 5.0f)
        return 0; // user failed
    int q = 5 - static_cast<int>(std::lround(user_difficulty));
    return std::clamp(q, 0, 5);
}


AlgorithmResult SM2Algorithm::process_answer(const Card& old_card, float user_difficulty) noexcept {
    AlgorithmResult r;
    r.updated_card = old_card; // copy

    const int q = quality_from_user_difficulty(user_difficulty);

    // read EF from difficulty
    float ef = difficulty_to_ef(old_card.difficulty);
    int   repetitions = 0; // NOTE: StudyService should manage and pass repetitions if needed
    // But since IAlgorithm interface does not receive repetitions, algorithm computes using only card.interval and ef.
    // We'll follow SM-2 logic using old_card.interval as previous interval and interpreting no explicit repetitions:
    // If old interval == 0 -> treat as first repetition.
    // If user failed (q < 3): repeat next day, set requeue true.
    if (q < 3) {
        // failure: reset repetitions to 0, schedule 1 day, mark incorrect
        r.updated_card.state = 2; // incorrect
        r.updated_card.incorrect_streak = old_card.incorrect_streak + 1;
        r.updated_card.interval = 1;
        r.updated_card.next_review = QDateTime::currentDateTime().addDays(1);
        // increase difficulty (towards 5.0) moderately
        r.updated_card.difficulty = std::clamp(old_card.difficulty + 0.30f * (5.0f - old_card.difficulty), 0.0f, 5.0f);
        r.requeue = true;
        return r;
    }

    // success path: quality >= 3
    r.updated_card.incorrect_streak = 0;

    // Update EF according to SM-2 formula:
    // EF' = EF + (0.1 - (5 - q) * (0.08 + (5 - q) * 0.02))
    float new_ef = ef + (0.1f - (5 - q) * (0.08f + (5 - q) * 0.02f));
    if (new_ef < 1.3f)
        new_ef = 1.3f;

    // compute new interval depending on previous interval:
    int prev_interval = std::max(0, old_card.interval);
    int new_interval = 0;
    if (prev_interval <= 0) {
        // first successful repetition
        new_interval = 1;
    } else if (prev_interval == 1) {
        new_interval = 6;
    } else {
        // general case
        new_interval = static_cast<int>(std::round(prev_interval * new_ef));
    }

    // cap interval to, say, 20 days to avoid overflow
    new_interval = std::clamp(new_interval, 1, 20);

    r.updated_card.interval = new_interval;
    r.updated_card.next_review = QDateTime::currentDateTime().addDays(new_interval);

    // write EF back into difficulty field
    r.updated_card.difficulty = ef_to_difficulty(new_ef);

    // set state to "consolidate"
    r.updated_card.state = 1;
    r.requeue = false; // move to trained list

    return r;
}

} // namespace revise
