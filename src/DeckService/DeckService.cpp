// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <DeckService/DeckService.hpp>

namespace revise {

// Public method
DeckService::DeckService(IDeckRepository& repository, QObject* parent) : QObject(parent), m_repository(repository) {}


// Public method
std::expected<QVector<DeckSummary>, QString> DeckService::list_decks(const QDateTime& now) {
    return m_repository.get_deck_summaries(now);
}

// Public method
void DeckService::create_deck(const QString& name,
                              const QString& description,
                              int            time_limit,
                              int            new_limit,
                              int            consolidate_limit,
                              int            incorrect_limit) {

    auto res = m_repository.create_deck(Deck{.name = name,
                                             .description = description,
                                             .time_limit = time_limit,
                                             .new_limit = new_limit,
                                             .consolidate_limit = consolidate_limit,
                                             .incorrect_limit = incorrect_limit});

    if (!res.has_value()) {
        emit errorOccured(res.error());
        return;
    }

    emit deckCreated();
    emit decksUpdated();
}

// Public method
void DeckService::delete_deck(int deckId)
{
    auto res = m_repository.delete_deck(deckId);

    if (!res.has_value()) {
        emit errorOccured(res.error());
        return;
    }

    emit deckRemoved();
    emit decksUpdated();
}




} // namespace revise
