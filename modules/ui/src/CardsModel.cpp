// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardsModel.hpp"

#include <QThreadPool> // for QThreadPool
#include <algorithm>   // for std::ranges::for_each
#include <cstddef>     // for std::size_t

namespace ui {

CardsModel::CardsModel(core::IDeckService& service, QObject* parent) : QAbstractListModel(parent), m_deck_service(service) {
    m_deck_service.connect<core::IDeckService::card_created>([this](auto& e) { on_card_created(e.card); });

    m_deck_service.connect<core::IDeckService::card_removed>([this](auto& e) { on_card_removed(e.card); });

    m_deck_service.connect<core::IDeckService::card_updated>([this](auto& e) { on_card_updated(e.card); });
}


int CardsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_cards.size());
}


QVariant CardsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_cards.size()) {
        return QVariant{};
    }

    const auto& card = m_cards.at(index.row());

    switch (role) {
    case FrontRole:
        return card.front();
    case BackRole:
        return card.back();
    case IdRole:
        return card.id();
    case NextReviewRole:
        return card.next_review();
    case DifficultyRole:
        return card.difficulty();
    case StatusRole:
        return static_cast<int>(card.state());
    case SelectedRole:
        return card.selected();
    default:
        return QVariant{};
    }
}


QHash<int, QByteArray> CardsModel::roleNames() const {
    static QHash<int, QByteArray> roles{{FrontRole, "front"},
                                        {BackRole, "back"},
                                        {IdRole, "id"},
                                        {NextReviewRole, "nextReview"},
                                        {DifficultyRole, "difficulty"},
                                        {StatusRole, "status"},
                                        {SelectedRole, "selected"}};

    return roles;
}


void CardsModel::setDeck(const int deckId) {
    m_last_deck_id.store(deckId);
    const int request_id = m_last_request_id.fetch_add(1) + 1;

    emit deckChanged(deckId);
    emit loadingStarted();

    QThreadPool::globalInstance()->start([this, deckId, request_id]() {
        auto res = m_deck_service.cards(deckId).result<>();

        if (!res) {
            qWarning() << "Failed to fetch cards in cards model:" << res.error().message;
            QMetaObject::invokeMethod(
                this,
                [this, request_id]() {
                    if (request_id == m_last_request_id.load()) {
                        emit loadingFinished();
                    }
                },
                Qt::QueuedConnection);
            return;
        }

        QVector<Card> converted;
        converted.reserve(res->size());
        for (const auto& c : *res) {
            converted.emplace_back(c);
        }

        QMetaObject::invokeMethod(
            this,
            [this, cards = std::move(converted), request_id]() {
                if (request_id != m_last_request_id.load()) {
                    return;
                }

                beginResetModel();
                m_cards = std::move(cards);
                endResetModel();

                emit updated();
                emit loadingFinished();
            },
            Qt::QueuedConnection);
    });
}


void CardsModel::selectCard(const int cardId) noexcept {
    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](auto& card) mutable {
        if (card.id() == cardId && !card.selected()) {
            card.set_selected(true);

            ++m_selected_count;
            const QModelIndex idx = index(static_cast<int>(i));
            emit              dataChanged(idx, idx, {SelectedRole});
            emit              cardSelectionChanged(cardId, true);
        }
        ++i;
    });
}


void CardsModel::deselectCard(int cardId) noexcept {
    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](auto& card) mutable {
        if (card.id() == cardId && card.selected()) {
            card.set_selected(false);

            --m_selected_count;
            const QModelIndex idx = index(static_cast<int>(i));
            emit              dataChanged(idx, idx, {SelectedRole});
            emit              cardSelectionChanged(cardId, false);
        }
        ++i;
    });
}


void CardsModel::selectAll() {
    if (m_cards.isEmpty()) {
        return;
    }

    m_selected_count = static_cast<int>(m_cards.size());

    std::ranges::for_each(m_cards, [this](auto& card) {
        card.set_selected(true);
        emit cardSelectionChanged(card.id(), true);
    });

    emit dataChanged(index(0), index(static_cast<int>(m_cards.size()) - 1), {SelectedRole});
}


void CardsModel::deselectAll() {
    if (m_cards.isEmpty()) {
        return;
    }

    m_selected_count = 0;

    std::ranges::for_each(m_cards, [this](auto& card) {
        card.set_selected(false);
        emit cardSelectionChanged(card.id(), false);
    });

    emit dataChanged(index(0), index(static_cast<int>(m_cards.size()) - 1), {SelectedRole});
}


int CardsModel::deck_id() const noexcept {
    return m_last_deck_id.load();
}


int CardsModel::cards_count() const noexcept {
    return static_cast<int>(m_cards.size());
}


int CardsModel::new_cards_count() const noexcept {
    const auto count = std::ranges::count_if(m_cards, [](const auto& card) {
        return card.state() == Card::State::New;
    });

    return static_cast<int>(count);
}


int CardsModel::review_cards_count() const noexcept {
    const auto count = std::ranges::count_if(m_cards, [](const auto& card) {
        return card.state() == Card::State::Review;
    });

    return static_cast<int>(count);
}


int CardsModel::incorrect_cards_count() const noexcept {
    const auto count = std::ranges::count_if(m_cards, [](const auto& card) {
        return card.state() == Card::State::Incorrect;
    });

    return static_cast<int>(count);
}


int CardsModel::selected_count() const noexcept {
    return m_selected_count;
}


void CardsModel::on_card_created(const core::Card& card) {
    if (card.deck_id != m_last_deck_id.load()) {
        return;
    }

    const int row = static_cast<int>(m_cards.size());

    beginInsertRows(QModelIndex(), row, row);
    m_cards.emplace_back(card);
    endInsertRows();

    emit updated();
}


void CardsModel::on_card_removed(const core::Card& card) {
    std::size_t row = m_cards.size();

    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](const auto& c) mutable {
        if (c.id() == card.id && row == static_cast<std::size_t>(m_cards.size())) {
            row = i;
        }
        ++i;
    });

    if (row == static_cast<std::size_t>(m_cards.size())) {
        return;
    }

    beginRemoveRows(QModelIndex(), static_cast<int>(row), static_cast<int>(row));
    m_cards.removeAt(static_cast<int>(row));
    endRemoveRows();

    emit updated();
}


void CardsModel::on_card_updated(const core::Card& card) {
    const bool  moved = card.deck_id != m_last_deck_id.load();
    std::size_t row = m_cards.size();

    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](auto& c) mutable {
        if (c.id() == card.id && row == static_cast<std::size_t>(m_cards.size())) {
            row = i;

            if (!moved) {
                const bool was_selected = c.selected();
                c = Card{card};
                c.set_selected(was_selected);

                const QModelIndex idx = index(static_cast<int>(i));
                emit              dataChanged(idx, idx);
            }
        }
        ++i;
    });

    if (!moved || row == static_cast<std::size_t>(m_cards.size())) {
        return;
    }

    beginRemoveRows(QModelIndex(), static_cast<int>(row), static_cast<int>(row));
    m_cards.removeAt(static_cast<int>(row));
    endRemoveRows();

    emit updated();
}

} // namespace ui
