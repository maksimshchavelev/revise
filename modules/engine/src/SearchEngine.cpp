// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/SearchEngine.hpp" // for header
#include <ranges>                  // std::ranges
#include <utils/Html.hpp>          // for html utils

namespace engine {

SearchEngine::SearchEngine(core::IDeckStorage& storage) : m_storage(storage) {}

std::expected<QVector<core::Card>, core::SearchError> SearchEngine::search_cards(
    const core::CardFilterChain& filters) const {

    auto cards = m_storage.fetch_cards();

    if (!cards) {
        return std::unexpected(
            core::SearchError{.kind = core::SearchError::Kind::BackendFailure, .cause = {}, .message = cards.error()});
    }

    auto filtered = cards.value() | std::ranges::views::filter([&](const core::Card& card) {
                        for (const auto& filter : filters.chain) {
                            if (!card_matches(card, filter)) {
                                return false;
                            }
                        }
                        return true;
                    });

    return QVector<core::Card>(filtered.begin(), filtered.end());
}


bool SearchEngine::card_matches(const core::Card& card, const std::unique_ptr<core::CardSearchFilter>& filter) const {
    switch (filter->kind()) {
    // By deck id
    case core::CardFilterKind::DeckId: {
        const auto& f = static_cast<const core::CardDeckIdSearchFilter&>(*filter);
        return card.deck_id == f.deck_id;
    }

    // By front
    case core::CardFilterKind::Front: {
        const auto& f = static_cast<const core::CardFrontSearchFilter&>(*filter);
        QString     plain = utils::Html::plain_text(card.front).trimmed();
        return plain.startsWith(f.front.trimmed(), Qt::CaseInsensitive);
    }

    // Unknown
    case core::CardFilterKind::Unknown: {
        qWarning() << "Unknown card filter type";
        return false;
    }
    }
}

} // namespace engine
