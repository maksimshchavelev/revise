// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardSearchModel.hpp"
#include "CardsModel.hpp" // for CardsModel::deck_id(), deckChanged, cardSelectionChanged
#include <QThreadPool> // for QThreadPool
#include <algorithm>   // for std::ranges::for_each
#include <cstddef>     // for std::size_t
#include <utility>     // for std::move

namespace ui {

CardSearchModel::CardSearchModel(core::IDeckService& service, CardsModel& cards_model, QObject* parent) :
    QAbstractListModel(parent), m_deck_service(service), m_cards_model(cards_model) {
    connect(&m_cards_model, &CardsModel::deckChanged, this, [this](int) {
        m_selected_ids.clear();
        reload();
    });

    connect(&m_cards_model, &CardsModel::cardSelectionChanged, this, &CardSearchModel::on_card_selection_changed);

    m_deck_service.connect<core::IDeckService::card_created>([this](auto& e) { on_card_created(e.card); });
    m_deck_service.connect<core::IDeckService::cards_removed>([this](auto& e) { on_cards_removed(e.cards); });
    m_deck_service.connect<core::IDeckService::cards_updated>([this](auto& e) { on_cards_updated(e.cards); });
}


int CardSearchModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_cards.size();
}


QVariant CardSearchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_cards.size()) {
        return QVariant();
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
        return m_selected_ids.contains(card.id());
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> CardSearchModel::roleNames() const {
    static QHash<int, QByteArray> roles{{FrontRole, "front"},
                                        {BackRole, "back"},
                                        {IdRole, "id"},
                                        {NextReviewRole, "nextReview"},
                                        {DifficultyRole, "difficulty"},
                                        {StatusRole, "status"},
                                        {SelectedRole, "selected"}};

    return roles;
}


QString CardSearchModel::search_front() const {
    return m_search_front;
}


void CardSearchModel::set_search_front(QString front) {
    if (m_search_front == front) {
        return;
    }

    m_search_front = std::move(front);
    emit searchFrontChanged();
    reload();
}


int CardSearchModel::found_cards_count() const noexcept {
    return m_cards.size();
}


void CardSearchModel::reload() {
    const int request_id = m_last_request_id.fetch_add(1) + 1;

    if (m_search_front.trimmed().isEmpty()) {
        if (!m_cards.isEmpty()) {
            beginResetModel();
            m_cards.clear();
            endResetModel();
            emit updated();
        }
        return;
    }

    const int     deckId = m_cards_model.deck_id();
    const QString query = m_search_front;

    emit loadingStarted();

    QThreadPool::globalInstance()->start([this, deckId, query, request_id]() {
        auto res =
            m_deck_service.search_cards(core::CardDeckIdSearchFilter{deckId} | core::CardFrontSearchFilter{query}).result<>();

        if (!res) {
            qWarning() << "Failed to search cards in card search model:" << res.error().message;
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


void CardSearchModel::on_card_created(const core::Card& card) {
    if (!matches(card)) {
        return;
    }

    const int row = m_cards.size();

    beginInsertRows(QModelIndex(), row, row);
    m_cards.emplace_back(card);
    endInsertRows();

    emit updated();
}


void CardSearchModel::on_cards_removed(const QVector<core::Card>& cards) {
    if (cards.isEmpty()) {
        return;
    }

    QSet<int> ids_to_remove;
    ids_to_remove.reserve(cards.size());
    std::ranges::for_each(cards, [&ids_to_remove](const core::Card& c) {
        ids_to_remove.insert(c.id);
    });

    std::ranges::for_each(ids_to_remove, [this](const int id) {
        m_selected_ids.remove(id);
    });

    if (m_cards.isEmpty()) {
        return;
    }

    QVector<int> rows_to_remove;
    rows_to_remove.reserve(ids_to_remove.size());

    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](const auto& c) mutable {
        if (ids_to_remove.contains(c.id())) {
            rows_to_remove.push_back(static_cast<int>(i));
        }
        ++i;
    });

    if (rows_to_remove.isEmpty()) {
        return;
    }

    std::ranges::reverse(rows_to_remove);
    std::ranges::for_each(rows_to_remove, [this](const int row) {
        beginRemoveRows(QModelIndex(), row, row);
        m_cards.removeAt(row);
        endRemoveRows();
    });

    emit updated();
}


void CardSearchModel::on_cards_updated(const QVector<core::Card>& cards) {
    if (cards.isEmpty() || m_cards.isEmpty()) {
        return;
    }

    const int current_deck = m_cards_model.deck_id();

    QHash<int, const core::Card*> updates;
    updates.reserve(cards.size());
    std::ranges::for_each(cards, [&updates](const core::Card& c) {
        updates.insert(c.id, &c);
    });

    QVector<int> rows_to_remove;

    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](auto& c) mutable {
        const auto it = updates.constFind(c.id());
        if (it == updates.constEnd()) {
            ++i;
            return;
        }

        const core::Card& updated = *it.value();
        if (updated.deck_id != current_deck) {
            rows_to_remove.push_back(static_cast<int>(i));
        } else {
            const bool was_selected = c.selected();
            c = Card{updated};
            c.set_selected(was_selected);

            const QModelIndex idx = index(static_cast<int>(i));
            emit dataChanged(idx, idx);
        }
        ++i;
    });

    if (rows_to_remove.isEmpty()) {
        return;
    }

    std::ranges::reverse(rows_to_remove);
    std::ranges::for_each(rows_to_remove, [this](const int row) {
        beginRemoveRows(QModelIndex(), row, row);
        m_cards.removeAt(row);
        endRemoveRows();
    });

    emit updated();
}


void CardSearchModel::on_card_selection_changed(int cardId, bool selected) {
    std::ranges::for_each(m_cards, [&, i = std::size_t{0}](auto& card) mutable {
        if (card.id() == cardId) {
            if (selected) {
                m_selected_ids.insert(cardId);
            } else {
                m_selected_ids.remove(cardId);
            }

            const QModelIndex idx = index(static_cast<int>(i));
            emit              dataChanged(idx, idx, {SelectedRole});
        }
        ++i;
    });
}


bool CardSearchModel::matches(const core::Card& card) const {
    if (m_search_front.trimmed().isEmpty()) {
        return false;
    }

    if (card.deck_id != m_cards_model.deck_id()) {
        return false;
    }

    return card.front.contains(m_search_front, Qt::CaseInsensitive);
}

} // namespace ui
