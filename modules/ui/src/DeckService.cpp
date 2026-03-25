// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "DeckService.hpp" // for header

namespace ui {

DeckService::DeckService(core::IDeckService& deck_service, QObject* parent) :
    QObject(parent), m_deck_service(deck_service) {}

void DeckService::create_deck(const Deck& deck) {
    auto res = m_deck_service.create_deck(deck);
    if (!res) {
        qWarning() << "ui::DeckService::create_deck() failed to create deck, cause:" << res.error();
    }
}

void DeckService::remove_deck(int deckId) {
    auto res = m_deck_service.remove_deck(deckId);
    if (!res) {
        qWarning() << "ui::DeckService::remove_deck() failed to remove deck, cause:" << res.error();
    }
}

void DeckService::import_deck_async(const QString& path) {
    m_deck_service.import_deck_async(path);
}

void DeckService::export_deck_async(int deckId, const QString& path) {
    m_deck_service.export_deck_async(deckId, path);
}

void DeckService::update_deck(const Deck& deck) {
    auto res = m_deck_service.update_deck(deck);
    if (!res) {
        qWarning() << "ui::DeckService::update_deck() failed to update deck, cause:" << res.error();
    }
}

void DeckService::create_card(const Card& card) {
    auto res = m_deck_service.create_card(card);
    if (!res) {
        qWarning() << "ui::DeckService::create_card() failed to create card, cause:" << res.error();
    }
}

void DeckService::remove_card(int id) {
    auto res = m_deck_service.remove_card(id);
    if (!res) {
        qWarning() << "ui::DeckService::remove_card() failed to remove card, cause:" << res.error();
    }
}

void DeckService::update_card(const Card& card) {
    auto res = m_deck_service.update_card(card);
    if (!res) {
        qWarning() << "ui::DeckService::update_card() failed to update card, cause:" << res.error();
    }
}

Deck DeckService::deck(int deckId)
{
    auto res = m_deck_service.deck(deckId);
    if (!res) {
        qWarning() << "ui::DeckService::deck() failed to fetch deck info with id" << deckId << "cause:" << res.error();
        return {};
    }

    return *res;
}

} // namespace ui
