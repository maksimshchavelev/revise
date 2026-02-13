// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QQueue>                 // for QQueue
#include <QTimer>                 // for QTimer
#include <core/IStudyEngine.hpp>  // for core::IStudyEngine
#include <core/IStudyService.hpp> // for core::IStudyService

namespace engine {

/**
 * @brief Learning engine implementation
 */
class StudyService final : public core::IStudyService {
  public:
    StudyService(core::IStudyEngine& study_engine);

    /// @copydoc core::IStudyService::start
    void start(int deck_id) override;

    /// @copydoc core::IStudyService::answer
    void answer(float difficulty) override;

    /// @copydoc core::IStudyService::flip
    void flip() override;

    /// @copydoc core::IStudyService::abort
    void abort() override;

    /// @copydoc core::IStudyService::current_card
    const std::optional<core::Card>& current_card() const override;

    /// @copydoc core::IStudyService::time_limit
    int time_limit() const override;

    /// @copydoc core::IStudyService::time_remaining
    float time_remaining() const override;

  private:
    core::IStudyEngine& m_study_engine;
    QTimer              m_timer;
};

} // namespace engine
