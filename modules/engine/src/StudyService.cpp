// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/StudyService.hpp" // for header

namespace engine {

StudyService::StudyService(core::IStudyEngine& study_engine) : m_study_engine(study_engine) {
    QObject::connect(&m_timer, &QTimer::timeout, [this]() {
        if (m_timer.isActive()) {
            if (!m_timer.isActive()) {
                return;
            }

            // Reduce time only if the card is not flipped
            if (!m_study_engine.state().flipped) {
                m_study_engine.state().time_remaining -= 0.1f; // - 0.1 sec

                if (m_study_engine.state().time_remaining <= 0.0f)
                    m_study_engine.state().time_remaining = 0.0f;

                dispatch(remaining_time_changed{m_study_engine.state().time_remaining});
            }

            if (m_study_engine.state().time_limit > 0 && m_study_engine.state().time_remaining <= 0.0f) {
                // schedule forced failure on next event loop to avoid reentrancy
                answer(5.0f /* max difficulty */);
            }
        }
    });

    m_timer.setInterval(100 /* msec */);
}


void StudyService::start(int deck_id) {
    auto res = m_study_engine.start(deck_id);

    if (!res) {
        dispatch(error{QString("Failed to start training, cause: %1").arg(res.error())});
        return;
    }

    if (m_study_engine.state().time_limit > 0) {
        m_timer.start();
    }

    dispatch(training_started{});
}


void StudyService::answer(float difficulty) {
    auto res = m_study_engine.answer(difficulty);

    if (!res) {
        dispatch(error{QString("Failed to answer card, cause: %1").arg(res.error())});
        return;
    }

    if (res->finished) {
        auto save_res = m_study_engine.save_trained();

        if (!save_res) {
            dispatch(error{QString("Training finished, failed to save results, cause: %1").arg(res.error())});
            return;
        }

        dispatch(training_finished{});
    }

    if (res->next_card) {
        m_study_engine.state().flipped = false;
        dispatch(flipped_changed{});
        dispatch(card_changed{});
    }
}


void StudyService::flip() {
    auto res = m_study_engine.flip();

    if (!res) {
        dispatch(error{QString("Failed to flip card, cause: %1").arg(res.error())});
        return;
    }

    dispatch(flipped_changed{});
}


void StudyService::abort() {
    auto res = m_study_engine.abort();

    if (!res) {
        dispatch(error{QString("Failed to abort training, cause: %1").arg(res.error())});
        return;
    }

    dispatch(training_aborted{});
}


const std::optional<core::Card>& StudyService::current_card() const {
    return m_study_engine.state().current_card;
}


int StudyService::time_limit() const {
    return m_study_engine.state().time_limit;
}


float StudyService::time_remaining() const {
    return m_study_engine.state().time_remaining;
}


bool StudyService::flipped() const {
    return m_study_engine.state().flipped;
}

} // namespace engine
