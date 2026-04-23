// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/StudyEngine.hpp" // for header
#include <QRandomGenerator>       // for QRandomGenerator
#include <algorithm>              // for algorithms
#include <ranges>                 // for std::ranges

namespace engine {

StudyEngine::StudyEngine(StudyEngineDeps deps) : m_deps(deps) {}


std::expected<void, QString> StudyEngine::start(int deck_id) {
    m_cards.clear();
    m_trained_cards.clear();

    m_session_state.current_card = std::nullopt;
    m_session_state.flipped = false;
    m_session_state.time_limit = 0;
    m_session_state.time_remaining = 0;

    auto fetched_cards = m_deps.deck_storage.fetch_cards(deck_id);

    if (!fetched_cards) {
        return std::unexpected(
            QString("Failed to fetch cards to start training, cause: %1").arg(fetched_cards.error()));
    }

    auto deck_info = m_deps.deck_storage.fetch_decks(QVector<int>{deck_id});

    if (!deck_info) {
        return std::unexpected(QString("Failed to fetch deck info, cause: %1").arg(deck_info.error()));
    }

    auto filtered_cards =
        fetched_cards.value() //
        | std::views::filter([](const auto& card) { return card.next_review <= QDateTime::currentDateTime(); });

    auto new_cards = filtered_cards                                                                              //
                     | std::views::filter([](const auto& card) { return card.state == core::Card::State::New; }) //
                     | std::views::take(deck_info->first().new_limit);                                           //

    auto consolidate_cards =
        filtered_cards                                                                                 //
        | std::views::filter([](const auto& card) { return card.state == core::Card::State::Review; }) //
        | std::views::take(deck_info->first().review_limit);                                           //

    auto incorrect_cards =
        filtered_cards                                                                                    //
        | std::views::filter([](const auto& card) { return card.state == core::Card::State::Incorrent; }) //
        | std::views::take(deck_info->first().incorrect_limit);                                           //

    for (const auto& card : new_cards) {
        m_cards.emplaceBack(std::move(card));
    }

    for (const auto& card : consolidate_cards) {
        m_cards.emplaceBack(std::move(card));
    }

    for (const auto& card : incorrect_cards) {
        m_cards.emplaceBack(std::move(card));
    }

    auto shuffle = [](QQueue<core::Card>& source) {
        QList<core::Card> list;

        while (!source.isEmpty()) {
            list.append(source.dequeue());
        }

        std::shuffle(list.begin(), list.end(), std::default_random_engine(QRandomGenerator::global()->generate()));

        for (const auto& item : list) {
            source.enqueue(item);
        }
    };

    if (m_cards.empty()) {
        return std::unexpected(QString("Failed to begin studying: nothing to study"));
    }

    shuffle(m_cards);

    m_session_state.current_card = m_cards.head();
    m_session_state.time_limit = deck_info->first().time_limit;
    m_session_state.time_remaining = m_session_state.time_limit;

    return {};
}


std::expected<void, QString> StudyEngine::flip() {
    m_session_state.flipped = !m_session_state.flipped;
    return {};
}


std::expected<void, QString> StudyEngine::reload_state() {
    if (!state().current_card) {
        return {};
    }

    auto card = m_deps.deck_storage.fetch_cards(QVector<int>{state().current_card->id});

    if (!card) {
        return std::unexpected(
            QString("Failed to reload study engine state: failed to fetch card: %1").arg(card.error()));
    }

    if (card->isEmpty()) {
        return std::unexpected(
            QString("Failed to reload study engine state: deck storage returned empty vector").arg(card.error()));
    }

    state().current_card = std::move(card->first());

    return {};
}


std::expected<void, QString> StudyEngine::abort() {
    m_cards.clear();
    m_trained_cards.clear();
    return {};
}


std::expected<void, QString> StudyEngine::save_trained() {
    auto res = m_deps.deck_storage.update_cards(m_trained_cards);

    if (!res) {
        return std::unexpected(QString("Failed to update cards, cause: %1").arg(res.error()));
    }

    return {};
}


std::expected<core::IStudyEngine::StudyInfo, QString> StudyEngine::get_study_info(int deck_id) const {
    auto fetched_cards = m_deps.deck_storage.fetch_cards(deck_id);

    if (!fetched_cards) {
        return std::unexpected(QString("Failed to fetch cards, cause: %1").arg(fetched_cards.error()));
    }

    auto filtered_cards =
        fetched_cards.value() //
        | std::views::filter([](const auto& card) { return card.next_review <= QDateTime::currentDateTime(); });

    auto deck_info = m_deps.deck_storage.fetch_decks(QVector<int>{deck_id});

    if (!deck_info) {
        return std::unexpected(QString("Failed to fetch deck info, cause: %1").arg(deck_info.error()));
    }

    // Count
    int new_cards = std::count_if(filtered_cards.begin(), filtered_cards.end(), [](const auto& card) {
        return card.state == core::Card::State::New; //
    });

    int consolidate_cards = std::count_if(filtered_cards.begin(), filtered_cards.end(), [](const auto& card) {
        return card.state == core::Card::State::Review; //
    });

    int incorrect_cards = std::count_if(filtered_cards.begin(), filtered_cards.end(), [](const auto& card) {
        return card.state == core::Card::State::Incorrent; //
    });

    return core::IStudyEngine::StudyInfo{
        .new_cards = new_cards,                     //
        .consolidate_cards = consolidate_cards,     //
        .incorrect_cards = incorrect_cards,         //
        .time_limit = deck_info->first().time_limit //
    };
}


StudyEngine::SessionState& StudyEngine::state() {
    return m_session_state;
}


std::expected<StudyEngine::StepResult, QString> StudyEngine::answer(float difficulty) {
    core::Card card = m_cards.dequeue();

    core::AlgorithmResult alg_res = m_deps.algorithm.process_answer(card, difficulty);

    if (alg_res.requeue) {
        m_cards.enqueue(alg_res.updated_card);
    } else {
        m_trained_cards.append(alg_res.updated_card);
    }

    m_session_state.flipped = false;

    if (m_cards.isEmpty()) {
        return StudyEngine::StepResult{.finished = true, .next_card = std::nullopt};
    }

    return StudyEngine::StepResult{.finished = false, .next_card = m_cards.head()};
}


} // namespace engine
