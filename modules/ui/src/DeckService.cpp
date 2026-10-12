// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "DeckService.hpp" // for header
#include <QFutureWatcher>  // for QFutureWatcher

namespace ui {

DeckService::DeckService(core::IDeckService& deck_service, QObject* parent) : QObject(parent), m_deck_service(deck_service) {}


void DeckService::create_deck(const DeckDraft& deck) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::create_deck() failed to create deck, cause:" << result.error().message;
        }

        watcher->deleteLater();
    });

    watcher->setFuture(m_deck_service.create_deck(deck));
}


void DeckService::remove_deck(const int deck_id) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::remove_deck() failed to remove deck, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.remove_deck(deck_id));
}


void DeckService::import_deck(const QString& path) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::import_deck() failed to import deck, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.import_deck(path));
}


void DeckService::export_deck(const int deck_id, const QString& path) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::export_deck() failed to export deck, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.export_deck(deck_id, path));
}


void DeckService::update_deck(const int deck_id, const DeckDraft& deck) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::update_deck() failed to update deck, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.update_deck(deck_id, deck));
}


void DeckService::create_card(const int deck_id, const CreateCardDraft& card) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::create_card() failed to create card, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.create_card(deck_id, card));
}


void DeckService::remove_card(const int card_id) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::remove_card() failed to remove card, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.remove_card(card_id));
}


void DeckService::update_card(const int deck_id, const UpdateCardDraft& card) {
    auto* watcher = new QFutureWatcher<core::IDeckService::Result<void>>(this);

    connect(watcher, &QFutureWatcher<core::IDeckService::Result<void>>::finished, this, [watcher] {
        if (const auto result = watcher->future().result(); !result) {
            qWarning() << "ui::DeckService::update_card() failed to update card, cause:" << result.error().message;
        }
    });

    watcher->setFuture(m_deck_service.update_card(deck_id, card));
}


Deck DeckService::deck(const int deck_id) const {
    const auto result = m_deck_service.deck(deck_id).result<core::IDeckService::Result<Deck>>();

    if (!result) {
        qWarning() << "ui::DeckService::deck() failed to fetch deck, cause:" << result.error().message;
        return Deck{};
    }

    return Deck{*result};
}

Card DeckService::card(const int card_id) const {
    const auto result = m_deck_service.card(card_id).result<core::IDeckService::Result<Card>>();

    if (!result) {
        qWarning() << "ui::DeckService::deck() failed to fetch deck, cause:" << result.error().message;
        return Card{};
    }

    return Card{*result};
}

} // namespace ui
