// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "DecksModel.hpp"

#include <algorithm> // for std::ranges::sort, std::ranges::for_each
#include <cstddef>   // for std::size_t
#include <utility>   // for std::move

namespace ui {

DecksModel::DecksModel(core::IDeckService& deck_service, core::IStudyService& study_service, QObject* parent) :
    QAbstractListModel(parent), m_deck_service(deck_service), m_study_service(study_service) {

    m_deck_service.connect<core::IDeckService::deck_created>([this](auto& e) { on_deck_created(e.deck); });
    m_deck_service.connect<core::IDeckService::deck_imported>([this](auto& e) { on_deck_created(e.deck); });

    m_deck_service.connect<core::IDeckService::deck_removed>([this](auto& e) { on_deck_removed(e.deck); });

    m_deck_service.connect<core::IDeckService::deck_updated>([this](auto& e) { on_deck_updated(e.deck); });
    m_deck_service.connect<core::IDeckService::deck_exported>([this](auto& e) { on_deck_updated(e.deck); });

    m_deck_service.connect<core::IDeckService::card_created>([this](auto& e) { on_card_created(e.card); });
    m_deck_service.connect<core::IDeckService::cards_removed>([this](auto& e) { on_cards_removed(e.cards); });
    m_deck_service.connect<core::IDeckService::cards_updated>([this](auto& e) { on_cards_updated(e.cards); });

    m_study_service.connect<core::IStudyService::training_finished>([this](auto&) { on_training_finished(); });

    reload();
}


int DecksModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_decks.size();
}


QVariant DecksModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_decks.size()) {
        return QVariant();
    }

    const core::DeckSummary& deck = m_decks.at(index.row());

    switch (role) {
    case NameRole:
        return deck.deck.name;
    case DescriptionRole:
        return deck.deck.description;
    case TimeLimitRole:
        return deck.deck.time_limit;
    case DeckId:
        return deck.deck.id;
    case NewCardsRole:
        return deck.new_cards;
    case ReviewCardsRole:
        return deck.review_cards;
    case IncorrectCardsRole:
        return deck.incorrect_cards;
    case RepeatableToday:
        return deck.new_cards > 0 || deck.review_cards > 0 || deck.incorrect_cards > 0;
    case IsSpecialRole:
        return index.row() == 0;
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> DecksModel::roleNames() const {
    static QHash<int, QByteArray> roles{{NameRole, "name"},
                                        {DescriptionRole, "description"},
                                        {TimeLimitRole, "timeLimit"},
                                        {NewCardsRole, "newCards"},
                                        {ReviewCardsRole, "reviewCards"},
                                        {IncorrectCardsRole, "incorrectCards"},
                                        {DeckId, "deckId"},
                                        {RepeatableToday, "repeatableToday"},
                                        {IsSpecialRole, "isSpecial"}};

    return roles;
}


void DecksModel::reload() {
    beginResetModel();

    m_decks.clear();
    m_decks.emplace_back(); // special deck. Must be first always

    if (auto res = m_deck_service.deck_summaries().result<>()) {
        std::ranges::sort(*res, [](const core::DeckSummary& a, const core::DeckSummary& b) {
            return a.new_cards + a.review_cards + a.incorrect_cards >
                   b.new_cards + b.review_cards + b.incorrect_cards;
        });
        m_decks.append(std::move(res.value()));
    }

    endResetModel();
}


int DecksModel::find_row(const int deckId) const noexcept {
    for (std::size_t i = 0; i < static_cast<std::size_t>(m_decks.size()); ++i) {
        if (m_decks[i].deck.id == deckId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}


void DecksModel::on_deck_created(const core::Deck& deck) {
    if (deck.id == 0) {
        return; // special deck is managed internally
    }

    if (find_row(deck.id) >= 0) {
        return; // already present
    }

    // Fetch the full summary for the new deck from the service.
    // This is the only reliable way to get the aggregated counters.
    if (auto res = m_deck_service.deck_summaries().result<>()) {
        auto it = std::ranges::find_if(*res, [&deck](const core::DeckSummary& s) {
            return s.deck.id == deck.id;
        });
        if (it != res->end()) {
            insert_deck_summary(*it);
        }
    }
}


void DecksModel::on_deck_removed(const core::Deck& deck) {
    const int row = find_row(deck.id);
    if (row < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_decks.removeAt(row);
    endRemoveRows();
}


void DecksModel::on_deck_updated(const core::Deck& deck) {
    const int row = find_row(deck.id);
    if (row < 0) {
        return;
    }

    // Update deck fields. Counters are not expected to change here,
    // but if the service can provide a fresh summary, use it.
    if (auto res = m_deck_service.deck_summaries().result<>()) {
        auto it = std::ranges::find_if(*res, [&deck](const core::DeckSummary& s) {
            return s.deck.id == deck.id;
        });
        if (it != res->end()) {
            update_deck_summary(row, *it);
            return;
        }
    }

    // Fallback: update only the deck part, keep counters.
    m_decks[row].deck = deck;
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {NameRole, DescriptionRole, TimeLimitRole, DeckId});
}


void DecksModel::on_card_created(const core::Card& card) {
    if (card.deck_id == 0) {
        return;
    }
    refresh_decks({card.deck_id});
}


void DecksModel::on_cards_removed(const QVector<core::Card>& cards) {
    QSet<int> affected;
    std::ranges::for_each(cards, [&affected](const core::Card& c) {
        if (c.deck_id != 0) {
            affected.insert(c.deck_id);
        }
    });
    if (!affected.isEmpty()) {
        refresh_decks(affected);
    }
}


void DecksModel::on_cards_updated(const QVector<core::Card>& cards) {
    QSet<int> affected;
    std::ranges::for_each(cards, [&affected](const core::Card& c) {
        if (c.deck_id != 0) {
            affected.insert(c.deck_id);
        }
    });
    if (!affected.isEmpty()) {
        refresh_decks(affected);
    }
}


void DecksModel::on_training_finished() {
    // Training can change counters for many decks. Fetch all summaries
    // and update only the rows whose data actually changed.
    if (auto res = m_deck_service.deck_summaries().result<>()) {
        // Build a map for quick lookup.
        QHash<int, const core::DeckSummary*> newSummaries;
        newSummaries.reserve(res->size());
        std::ranges::for_each(*res, [&newSummaries](const core::DeckSummary& s) {
            newSummaries.insert(s.deck.id, &s);
        });

        for (int row = 1; row < m_decks.size(); ++row) { // skip special deck
            const int id = m_decks[row].deck.id;
            auto it = newSummaries.constFind(id);
            if (it != newSummaries.constEnd()) {
                if (m_decks[row] != *it.value()) {
                    update_deck_summary(row, *it.value());
                }
            }
        }
    }
}


void DecksModel::refresh_decks(const QSet<int>& affectedDeckIds) {
    if (affectedDeckIds.isEmpty()) {
        return;
    }

    if (auto res = m_deck_service.deck_summaries().result<>()) {
        QHash<int, const core::DeckSummary*> newSummaries;
        newSummaries.reserve(res->size());
        std::ranges::for_each(*res, [&newSummaries](const core::DeckSummary& s) {
            newSummaries.insert(s.deck.id, &s);
        });

        std::ranges::for_each(affectedDeckIds, [this, &newSummaries](const int deckId) {
            const int row = find_row(deckId);
            if (row < 0) {
                return;
            }
            auto it = newSummaries.constFind(deckId);
            if (it != newSummaries.constEnd()) {
                update_deck_summary(row, *it.value());
            }
        });
    }
}


void DecksModel::insert_deck_summary(const core::DeckSummary& summary) {
    // Find insertion position: after the special deck, preserving sort order.
    int insertPos = 1; // after special deck
    const int total = summary.new_cards + summary.review_cards + summary.incorrect_cards;

    for (; insertPos < m_decks.size(); ++insertPos) {
        const int otherTotal = m_decks[insertPos].new_cards +
                               m_decks[insertPos].review_cards +
                               m_decks[insertPos].incorrect_cards;
        if (total > otherTotal) {
            break;
        }
    }

    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_decks.insert(insertPos, summary);
    endInsertRows();
}


void DecksModel::update_deck_summary(const int row, const core::DeckSummary& summary) {
    if (row < 0 || row >= m_decks.size()) {
        return;
    }

    if (m_decks[row] == summary) {
        return; // nothing changed
    }

    m_decks[row] = summary;

    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {NameRole, DescriptionRole, TimeLimitRole,
                                NewCardsRole, ReviewCardsRole, IncorrectCardsRole,
                                DeckId, RepeatableToday});
}

} // namespace ui
