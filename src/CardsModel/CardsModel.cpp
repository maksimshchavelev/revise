// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A model that stores a list of decks. Can be used in QML.

#include <CardsModel/CardsModel.hpp>
#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter

namespace revise {

// Public method
CardsModel::CardsModel(DeckService& service) : m_deck_service(service) {
    QObject::connect(&m_deck_service, &DeckService::deckUpdated, this, [this]() { load(m_last_deckId); });
}

// Public method
int CardsModel::rowCount(const QModelIndex& parent) const {
    return m_cards.size();
}

// Public method
QVariant CardsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_cards.size()) {
        return QVariant();
    }

    const Card& card = m_cards.at(index.row());

    switch (role) {
    case FrontRole:
        return card.front;
    case BackRole:
        return card.back;
    case IdRole:
        return card.id;
    default:
        return QVariant();
    }
}

// Public method
QHash<int, QByteArray> CardsModel::roleNames() const {
    static QHash<int, QByteArray> roles{{FrontRole, "front"},
                                        {BackRole, "back"},
                                        {IdRole, "id"}};

    return roles;
}

// Public method
void CardsModel::load(int deckId) {
    m_last_deckId = deckId;

    beginResetModel();

    auto res = m_deck_service.list_cards(deckId);

    if (res.has_value()) {
        m_cards = std::move(res.value());
    }

    endResetModel();
}

// Public method
int CardsModel::cards_count() const noexcept {
    return m_cards.size();
}

} // namespace revise
