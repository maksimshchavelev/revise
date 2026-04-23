// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/Card.hpp>                      // for core::Card
#include <core/Property.hpp>                  // for PROPERTY macro for getters and setters

namespace ui {

/// @brief Wrapper over core::Card, visible from QML
struct CardWrapper {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(int id READ id WRITE set_id NOTIFY updated)
    Q_PROPERTY(int globalId READ global_id WRITE set_global_id NOTIFY updated)
    Q_PROPERTY(int deckId READ deck_id WRITE set_deck_id NOTIFY updated)
    Q_PROPERTY(int incorrectStreak READ incorrect_streak WRITE set_incorrect_streak NOTIFY updated)
    Q_PROPERTY(int interval READ interval WRITE set_interval NOTIFY updated)
    Q_PROPERTY(float difficulty READ difficulty WRITE set_difficulty NOTIFY updated)
    Q_PROPERTY(float timeLimit READ time_limit WRITE set_time_limit NOTIFY updated)
    Q_PROPERTY(QDateTime nextReview READ next_review WRITE set_next_review NOTIFY updated)
    Q_PROPERTY(QDateTime createdAt READ created_at WRITE set_created_at NOTIFY updated)
    Q_PROPERTY(QDateTime updatedAt READ updated_at WRITE set_updated_at NOTIFY updated)
    Q_PROPERTY(QString front READ front WRITE set_front NOTIFY updated)
    Q_PROPERTY(QString back READ back WRITE set_back NOTIFY updated)

  public:
    CardWrapper() = default;

    CardWrapper(const core::Card& card) : m_card{card} {}
    CardWrapper(core::Card&& card) : m_card{std::move(card)} {}

    CardWrapper(const CardWrapper&) = default;
    CardWrapper(CardWrapper&&) = default;
    CardWrapper& operator=(const CardWrapper&) = default;
    CardWrapper& operator=(CardWrapper&&) = default;

    CardWrapper& operator=(const core::Card& card) {
        m_card = card;
        return *this;
    }

    CardWrapper& operator=(core::Card&& card) {
        m_card = std::move(card);
        return *this;
    }

    PROPERTY(id, m_card.id);
    PROPERTY(global_id, m_card.global_id)
    PROPERTY(deck_id, m_card.deck_id)
    PROPERTY(difficulty, m_card.difficulty)
    PROPERTY(state, m_card.state)
    PROPERTY(incorrect_streak, m_card.incorrect_streak)
    PROPERTY(interval, m_card.interval)
    PROPERTY(time_limit, m_card.time_limit)
    PROPERTY(next_review, m_card.next_review)
    PROPERTY(created_at, m_card.created_at)
    PROPERTY(updated_at, m_card.updated_at)
    PROPERTY(front, m_card.front)
    PROPERTY(back, m_card.back)

  private:
    core::Card m_card;
};

} // namespace ui
