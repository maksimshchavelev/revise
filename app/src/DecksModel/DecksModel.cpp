// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A model that stores a list of decks. Can be used in QML.

#include <DecksModel/DecksModel.hpp>
#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter

namespace revise {

// Public method
DecksModel::DecksModel(DeckService& service) : m_deck_service(service) {
    QObject::connect(&m_deck_service, &DeckService::decksUpdated, this, [this]() { update(); });
    QObject::connect(&m_deck_service, &DeckService::deckUpdated, this, [this]() { update(); });

    update();
}

// Public method
int DecksModel::rowCount(const QModelIndex& parent) const {
    return m_decks.size();
}

// Public method
QVariant DecksModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_decks.size()) {
        return QVariant();
    }

    const DeckSummary& deck = m_decks.at(index.row());

    switch (role) {
    case NameRole:
        return deck.deck.name;
    case TimeLimitRole:
        return deck.deck.time_limit;
    case DeckId:
        return deck.deck.id;
    case NewCardsRole:
        return deck.new_cards;
    case ConsolidateCardsRole:
        return deck.consolidate_cards;
    case IncorrectCardsRole:
        return deck.incorrect_cards;
    case RepeatableToday:
        return deck.new_cards > 0 || deck.consolidate_cards > 0 || deck.incorrect_cards > 0;
    default:
        return QVariant();
    }
}

// Public method
QHash<int, QByteArray> DecksModel::roleNames() const {
    static QHash<int, QByteArray> roles{{NameRole, "name"},
                                        {TimeLimitRole, "timeLimit"},
                                        {NewCardsRole, "newCards"},
                                        {ConsolidateCardsRole, "consolidateCards"},
                                        {IncorrectCardsRole, "incorrectCards"},
                                        {DeckId, "deckId"},
                                        {RepeatableToday, "repeatableToday"}};

    return roles;
}


// Public method
void DecksModel::update() {
    beginResetModel();

    auto res = m_deck_service.list_decks(QDateTime::currentDateTime());

    if (res.has_value()) {
        m_decks = std::move(res.value());
    }

    endResetModel();
}


} // namespace revise
