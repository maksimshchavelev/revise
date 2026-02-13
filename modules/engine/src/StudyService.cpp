// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/StudyService.hpp" // for header

namespace engine {

StudyService::StudyService(core::IStudyEngine& study_engine) : m_study_engine(study_engine) {}


void StudyService::start(int deck_id)
{
    auto res = m_study_engine.start(deck_id);

    if (!res) {
        dispatch(error{QString("Failed to start training, cause: %1").arg(res.error())});
        return;
    }

    const auto state = m_study_engine.state();


}



} // namespace engine
