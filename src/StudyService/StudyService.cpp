// Copyright 2025 Maksim Shchavelev
// StudyService implementation (refactored StudyController)

#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter
#include <QRandomGenerator>                // for QRandomGenerator
#include <StudyService/StudyService.hpp>   // for StudyService

namespace revise {

// Constants
static constexpr float FAILURE_THRESHOLD = 5.0f;   // user reports failure when >= 5.0
static constexpr float TIMER_INTERVAL_MS = 100.0f; // tick every 100 ms
static constexpr float TIMER_DECREMENT_S = 0.1f;   // seconds per tick

StudyService::StudyService(StudyServiceDeps deps, QObject* parent) :
    QObject(parent), m_repo(deps.deck_repository), m_algo(deps.algorithm), m_event_recorder(deps.event_recorder),
    m_streak(deps.streak), m_timer(this) {
    m_timer.setSingleShot(false);
    m_timer.setInterval(static_cast<int>(TIMER_INTERVAL_MS));
    connect(&m_timer, &QTimer::timeout, this, &StudyService::on_timer_timeout);
}

void StudyService::enqueue_all(const QList<Card>& list) {
    for (const Card& c : list)
        m_cards.enqueue(c);
}

QString StudyService::card_text() const noexcept {
    if (m_cards.isEmpty())
        return QString();
    const Card& head = m_cards.head();
    return m_flipped ? head.back : head.front;
}

bool StudyService::flipped() const noexcept {
    return m_flipped;
}

int StudyService::time_limit() const noexcept {
    return m_time_limit;
}

float StudyService::time_remaining() const noexcept {
    return m_time_remaining;
}

void StudyService::on_timer_timeout() {
    if (m_timer_paused) {
        return;
    }

    // Reduce time only if the card is not flipped
    if (!m_flipped) {
        m_time_remaining -= TIMER_DECREMENT_S;
        if (m_time_remaining < 0.0f)
            m_time_remaining = 0.0f;
        emit time_remaining_changed();
    }

    if (m_time_limit > 0 && m_time_remaining <= 0.0f) {
        // schedule forced failure on next event loop to avoid reentrancy
        QMetaObject::invokeMethod(this, [this]() { this->next_card(FAILURE_THRESHOLD); }, Qt::QueuedConnection);
    }
}

void StudyService::start_training(int deck_id) {
    m_cards.clear();
    m_trained.clear();
    m_flipped = false;
    emit flipped_changed();

    auto cards_res = m_repo.get_cards(deck_id);
    if (!cards_res.has_value()) {
        ErrorReporter::instance()->report("Failed to fetch cards", cards_res.error(), "StudyService::start_training()");
        return;
    }

    QList<Card> failed;
    QList<Card> review;
    QList<Card> fresh;

    const QList<Card> cards = cards_res.value();
    const QDateTime   now = QDateTime::currentDateTime();

    for (const Card& card : cards) {
        if (card.next_review >= now) {
            continue; // skip
        }

        switch (card.state) {
        case 2:
            failed.push_back(card);
            break;
        case 1:
            review.push_back(card);
            break;
        case 0:
            fresh.push_back(card);
            break;
        default:
            break;
        }
    }

    // get deck info (limits/time)
    auto deck_res = m_repo.get_deck(deck_id);
    int  new_limit = 0;
    int  consolidate_limit = 0;
    int  incorrect_limit = 0;
    int  deck_time_limit = 0;

    if (deck_res.has_value()) {
        const Deck& d = deck_res.value();
        new_limit = d.new_limit;
        consolidate_limit = d.consolidate_limit;
        incorrect_limit = d.incorrect_limit;
        deck_time_limit = d.time_limit;
    } else {
        ErrorReporter::instance()->report("Failed to get deck info", deck_res.error(), "StudyService::start_training");
        return;
    }

    // sort review by oldest next_review first
    std::sort(review.begin(), review.end(), [](const Card& a, const Card& b) { return a.next_review < b.next_review; });

    // apply limits (same logic as legacy)
    if (incorrect_limit > 0 && failed.size() > incorrect_limit)
        failed = failed.mid(0, incorrect_limit);

    if (consolidate_limit > 0 && review.size() > consolidate_limit)
        review = review.mid(0, consolidate_limit);

    if (new_limit > 0 && fresh.size() > new_limit)
        fresh = fresh.mid(0, new_limit);

    // enqueue in legacy order: failed, review, fresh
    enqueue_all(failed);
    enqueue_all(review);
    enqueue_all(fresh);

    // shuffle queue
    auto shuffle = [](QQueue<Card>& source) {
        QList<Card> list;

        while (!source.isEmpty()) {
            list.append(source.dequeue());
        }

        std::shuffle(list.begin(), list.end(), std::default_random_engine(QRandomGenerator::global()->generate()));

        for (const auto& item : list) {
            source.enqueue(item);
        }
    };

    shuffle(m_cards);

    // timer setup
    m_time_limit = deck_time_limit;
    m_time_remaining = static_cast<float>(m_time_limit);

    if (m_time_limit > 0) {
        if (!m_timer.isActive())
            m_timer.start();
    } else {
        if (m_timer.isActive())
            m_timer.stop();
    }

    m_current_deck_id = deck_id;

    emit card_text_changed();
    emit time_limit_changed();
    emit time_remaining_changed();
}

void StudyService::next_card(float current_difficulty) {
    // Record last answer
    if (auto res = m_event_recorder.record(CardReviewEvent{
            .card_id = m_cards.head().id,
            .global_card_id = 0, // TODO: INSERT REAL VALUE
            .deck_id = m_current_deck_id,
            .global_deck_id = 0, // TODO: INSERT REAL VALUE
            .current_streak = m_streak,
            .review_duration = std::chrono::milliseconds(0) // TODO: INSERT REAL VALUE
        });
        !res.has_value()) {
        ErrorReporter::instance()->report("Failed to save review card event", res.error(), "StudyService::next_card");
    }

    // Dequeue current card
    Card card = m_cards.dequeue();

    // Delegate scheduling decision to algorithm
    AlgorithmResult alg_res = m_algo.process_answer(card, current_difficulty);

    // Apply algorithm result
    if (alg_res.requeue) {
        // Put updated card to the end of the queue
        m_cards.enqueue(alg_res.updated_card);
    } else {
        // Move to trained
        m_trained.append(alg_res.updated_card);
    }

    // Reset flip/time for next card
    m_flipped = false;
    m_time_remaining = static_cast<float>(m_time_limit);

    // If no cards left -> finish and persist trained
    if (m_cards.isEmpty()) {
        for (const Card& c : m_trained) {
            auto res = m_repo.update_card(c);
            if (!res.has_value()) {
                ErrorReporter::instance()->report("Failed to update card",
                                                  QString("Card IS: %1, cause: %2").arg(c.id).arg(res.error()),
                                                  "StudyService::next_card");
                return;
            }
        }

        if (m_timer.isActive())
            m_timer.stop();
        emit training_finished();
        emit card_text_changed();
        emit flipped_changed();
        emit time_remaining_changed();
        return;
    }

    // Notify UI to update to next card
    emit card_text_changed();
    emit flipped_changed();
    emit time_remaining_changed();
}

void StudyService::flip() noexcept {
    m_flipped = true;
    m_time_remaining = static_cast<float>(m_time_limit);
    emit flipped_changed();
    emit card_text_changed();
    emit time_remaining_changed();
}

void StudyService::abort() {
    if (m_timer.isActive())
        m_timer.stop();
    m_flipped = false;
    m_timer_paused = false;
    m_time_remaining = static_cast<float>(m_time_limit);
    emit aborted();
}

void StudyService::pause() {
    m_timer_paused = true;
}

void StudyService::resume() {
    m_timer_paused = false;
}

StudyService::StudyInfo StudyService::get_deck_study_info(int deck_id) {
    StudyInfo info;

    auto cards_res = m_repo.get_cards(deck_id);
    if (!cards_res.has_value()) {
        qWarning() << "StudyService::get_deck_study_info():" << cards_res.error();
        return info;
    }

    auto deck_res = m_repo.get_deck(deck_id);
    if (!deck_res.has_value()) {
        qWarning() << "StudyService::get_deck_study_info():" << deck_res.error();
        return info;
    }

    const QList<Card> cards = cards_res.value();
    const QDateTime   now = QDateTime::currentDateTime();

    for (const Card& card : cards) {
        if (now < card.next_review)
            continue;

        switch (card.state) {
        case 0:
            ++info.new_cards;
            break;
        case 1:
            ++info.consolidate_cards;
            break;
        case 2:
            ++info.incorrect_cards;
            break;
        default:
            break;
        }
    }

    if (!cards.isEmpty())
        info.time_limit = cards.first().time_limit;

    const Deck& deck = deck_res.value();
    info.new_cards = qMin(info.new_cards, deck.new_limit);
    info.consolidate_cards = qMin(info.consolidate_cards, deck.consolidate_limit);
    info.incorrect_cards = qMin(info.incorrect_cards, deck.incorrect_limit);

    return info;
}

} // namespace revise
