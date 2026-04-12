// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardsModel.hpp"            // for header
#include <QtConcurrent/QtConcurrent> // for QtConcurrent

namespace ui {

CardsModel::CardsModel(core::IDeckService& service, QObject* parent) :
    QAbstractListModel(parent), m_deck_service(service) {

    m_deck_service.connect<core::IDeckService::cards_updated>([this](auto& e) { setDeck(m_last_deck_id); });

    m_deck_service.connect<core::IDeckService::card_removed>([this](auto& e) { setDeck(m_last_deck_id); });

    m_deck_service.connect<core::IDeckService::card_created>([this](auto& e) { setDeck(m_last_deck_id); });
}


int CardsModel::rowCount(const QModelIndex& parent) const {
    return m_cards.size();
}


QVariant CardsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_cards.size()) {
        return QVariant();
    }

    const core::Card& card = m_cards.at(index.row());

    switch (role) {
    case FrontRole:
        return card.front;
    case BackRole:
        return card.back;
    case IdRole:
        return card.id;
    case NextReviewRole:
        return card.next_review;
    case DifficultyRole:
        return card.difficulty;
    case StatusRole:
        return static_cast<int>(card.state);
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> CardsModel::roleNames() const {
    static QHash<int, QByteArray> roles{{FrontRole, "front"},
                                        {BackRole, "back"},
                                        {IdRole, "id"},
                                        {NextReviewRole, "nextReview"},
                                        {DifficultyRole, "difficulty"},
                                        {StatusRole, "status"}};

    return roles;
}


void CardsModel::setDeck(int deckId) {
    m_last_deck_id = deckId;
    ++m_last_request_id;

    auto _ = QtConcurrent::run([this, deckId, search = m_search_front]() {
        int current_request_id = m_last_request_id;

        emit loadingStarted();

        if (m_search_front.trimmed().isEmpty()) {
            auto res = m_deck_service.cards(m_last_deck_id);

            if (!res) {
                qWarning() << "Failed to fetch cards in cards model:" << res.error();
                emit loadingFinished();
                return;
            }

            if (current_request_id == m_last_request_id) {
                QMetaObject::invokeMethod(this, [this, cards = std::move(*res)]() {
                    beginResetModel();
                    m_cards = std::move(cards);
                    emit updated();
                    endResetModel();
                    emit loadingFinished();
                }, Qt::QueuedConnection);
            }
        } else {
            auto res = m_deck_service.search_cards(core::CardDeckIdSearchFilter{m_last_deck_id} |
                                                   core::CardFrontSearchFilter{m_search_front});

            if (!res) {
                qWarning() << "Failed to fetch searched cards in cards model:" << res.error().message;
                emit loadingFinished();
                return;
            }

            if (current_request_id == m_last_request_id) {
                QMetaObject::invokeMethod(this, [this, cards = std::move(*res)]() {
                    beginResetModel();
                    m_cards = std::move(cards);
                    emit updated();
                    endResetModel();
                    emit loadingFinished();
                }, Qt::QueuedConnection);
            }
        }
    });
}


int CardsModel::cards_count() const noexcept {
    return m_cards.size();
}


QString CardsModel::search_front() const {
    return m_search_front;
}

void CardsModel::set_search_front(QString front) {
    m_search_front = std::move(front);
    setDeck(m_last_deck_id);
    emit searchFrontChanged();
}

} // namespace ui
