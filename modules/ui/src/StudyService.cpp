// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "StudyService.hpp" // for header

namespace ui {

StudyService::StudyService(core::IStudyService& study_service, QObject* parent) :
    QObject(parent), m_study_service(study_service) {
    m_study_service.connect<core::IStudyService::training_started>([this](const auto& e) {
        emit time_limit_changed();
        emit time_remaining_changed();
        emit back_changed();
        emit front_changed();
        emit flipped_changed();
    });

    m_study_service.connect<core::IStudyService::remaining_time_changed>(
        [this](const auto& e) { emit time_remaining_changed(); });

    m_study_service.connect<core::IStudyService::card_changed>([this](const auto& e) {
        emit back_changed();
        emit front_changed();
    });

    m_study_service.connect<core::IStudyService::flipped_changed>([this](const auto& e) { emit flipped_changed(); });
}


void StudyService::start(int deck_id) {
    m_study_service.start(deck_id);
}


void StudyService::answer(float difficulty) {
    m_study_service.answer(difficulty);
}


void StudyService::abort() {
    m_study_service.abort();
}


int StudyService::time_remaining() const {
    return m_study_service.time_remaining();
}


int StudyService::time_limit() const {
    return m_study_service.time_limit();
}


QString StudyService::front() const {
    auto res = m_study_service.current_card();

    if (!res) {
        return QString();
    }

    return res->front;
}


QString StudyService::back() const {
    auto res = m_study_service.current_card();

    if (!res) {
        return QString();
    }

    return res->back;
}


bool StudyService::flipped() const {
    return m_study_service.flipped();
}

} // namespace ui