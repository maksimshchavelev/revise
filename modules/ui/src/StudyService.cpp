// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "StudyService.hpp" // for header
#include <utils/Html.hpp>   // for Html utils

namespace ui {

StudyService::StudyService(core::IStudyService& study_service, QObject* parent) :
    QObject(parent), m_study_service(study_service) {
    m_study_service.connect<core::IStudyService::training_started>([this](const auto& e) {
        emit time_limit_changed();
        emit time_remaining_changed();
        emit back_changed();
        emit front_changed();
        emit flipped_changed();
        emit card_changed();
    });

    m_study_service.connect<core::IStudyService::remaining_time_changed>(
        [this](const auto& e) { emit time_remaining_changed(); });

    m_study_service.connect<core::IStudyService::card_changed>([this](const auto& e) {
        emit back_changed();
        emit front_changed();
        emit card_changed();
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


void StudyService::flip() {
    m_study_service.flip();
}


void StudyService::pause() {
    m_study_service.pause();
}


void StudyService::resume() {
    m_study_service.resume();
}


void StudyService::reloadCurrentCard() {
    m_study_service.reload_current_card();

    emit back_changed();
    emit front_changed();
}


float StudyService::time_remaining() const {
    return m_study_service.time_remaining();
}


int StudyService::time_limit() const {
    return m_study_service.time_limit();
}


QString StudyService::front() const {
    auto res = m_study_service.current_card();

    if (!res) {
        qWarning() << "StudyService::back(): failed to get card front";
        return QString();
    }

    if (utils::Html::new_lines(res->front) < 3) {
        res->front = utils::Html::center_horizontal(res->front);
    }

    res->front = utils::Html::center_vertical(res->front);

    return res->front;
}


QString StudyService::back() const {
    auto res = m_study_service.current_card();

    if (!res) {
        qWarning() << "StudyService::back(): failed to get card back";
        return QString();
    }

    if (utils::Html::new_lines(res->back) < 3) {
        res->back = utils::Html::center_horizontal(res->back);
    }

    res->back = utils::Html::center_vertical(res->back);

    return res->back;
}


bool StudyService::flipped() const {
    return m_study_service.flipped();
}


Card StudyService::card() {
    auto res = m_study_service.current_card();

    if (!res) {
        qWarning() << "Failed to get card in ui::StudyService (got nullopt)";
        return Card{};
    }

    return *res;
}

} // namespace ui