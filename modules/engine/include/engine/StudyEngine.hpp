// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QQueue>                // for QQueue
#include <core/IAlgorithm.hpp>   // for core::IAlgorithm
#include <core/IDeckStorage.hpp> // for core::IDeckStorage
#include <core/IStudyEngine.hpp> // for core::IStudyEngine

namespace engine {

/**
 * @brief The structure describes dependencies for `engine::StudyEngine`.
 */
struct StudyEngineDeps {
    core::IAlgorithm&   algorithm;    ///< Study algorithm
    core::IDeckStorage& deck_storage; ///< Deck storage
};

/**
 * @brief Learning engine implementation
 */
class StudyEngine final : public core::IStudyEngine {
  public:
    StudyEngine(StudyEngineDeps deps);

    /// @copydoc core::IStudyEngine::start
    std::expected<void, QString> start(int deck_id) override;

    /// @copydoc core::IStudyEngine::answer
    std::expected<StepResult, QString> answer(float difficulty) override;

    /// @copydoc core::IStudyEngine::flip
    std::expected<void, QString> flip() override;

    /// @copydoc core::IStudyEngine::state
    const SessionState& state() const override;

    /// @copydoc core::IStudyEngine::abort
    std::expected<void, QString> abort() override;

    /// @copydoc core::IStudyEngine::save_trained
    std::expected<void, QString> save_trained() override;

    /// @copydoc core::IStudyEngine::get_study_info
    std::expected<StudyInfo, QString> get_study_info(int deck_id) const override;

  private:
    QQueue<core::Card>               m_cards;
    QVector<core::Card>              m_trained_cards;
    core::IStudyEngine::SessionState m_session_state;
    StudyEngineDeps                  m_deps;
};

} // namespace engine
