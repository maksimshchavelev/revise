// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "CardsModel.hpp" // for header

namespace ui {

CardsModel::CardsModel(core::IDeckService& service, QObject* parent) :
    QAbstractListModel(parent), m_deck_service(service) {
    m_deck_service.connect<core::IDeckService::cards_updated>([this](const core::IDeckService::cards_updated& e) {
        setDeck(m_last_deck_id); // reload
    });
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
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> CardsModel::roleNames() const {
    static QHash<int, QByteArray> roles{{FrontRole, "front"}, {BackRole, "back"}, {IdRole, "id"}};

    return roles;
}


void CardsModel::setDeck(int deckId) {
    m_last_deck_id = deckId;

    beginResetModel();
    auto res = m_deck_service.cards(m_last_deck_id);

    if (res.has_value()) {
        m_cards = std::move(res.value());
    }

    endResetModel();
    emit updated();
}


int CardsModel::cards_count() const noexcept {
    return m_cards.size();
}

} // namespace ui
