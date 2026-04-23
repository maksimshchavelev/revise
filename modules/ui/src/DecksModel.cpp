// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "DecksModel.hpp" // for header

namespace ui {

DecksModel::DecksModel(core::IDeckService& deck_service, core::IStudyService& study_service, QObject* parent) :
    QAbstractListModel(parent), m_deck_service(deck_service), m_study_service(study_service) {

    m_deck_service.connect<core::IDeckService::decks_updated>([this](auto& e) { update(); });

    m_deck_service.connect<core::IDeckService::card_created>([this](auto& e) { update(); });

    m_deck_service.connect<core::IDeckService::card_removed>([this](auto& e) { update(); });

    m_study_service.connect<core::IStudyService::training_finished>([this](auto& e) { update(); });

    update();
}


int DecksModel::rowCount(const QModelIndex& parent) const {
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
    case ConsolidateCardsRole:
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
                                        {ConsolidateCardsRole, "consolidateCards"},
                                        {IncorrectCardsRole, "incorrectCards"},
                                        {DeckId, "deckId"},
                                        {RepeatableToday, "repeatableToday"},
                                        {IsSpecialRole, "isSpecial"}};

    return roles;
}


void DecksModel::update() {
    beginResetModel();

    auto res = m_deck_service.deck_summaries();

    if (res) {
        std::sort(res->begin(), res->end(), [](const core::DeckSummary& deck1, const core::DeckSummary& deck2) {
            return deck1.new_cards + deck1.review_cards + deck1.incorrect_cards >
                   deck2.new_cards + deck2.review_cards + deck2.incorrect_cards;
        });

        m_decks.clear();
        m_decks.emplace_back(); // special deck. Must be first always
        m_decks.append(std::move(res.value()));
    }

    endResetModel();
}

} // namespace ui