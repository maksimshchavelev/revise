// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>     // for QString
#include <memory>      // for std::make_unique, std::unique_ptr
#include <type_traits> // for type_traits
#include <vector>      // for std::vector

namespace core {

/// @brief Card filter kind
enum class CardFilterKind { DeckId, Front, Unknown };

/// @brief The base class for card search filters. All card filters must inherit from it
struct CardSearchFilter {
    virtual ~CardSearchFilter() = default;

    virtual CardFilterKind kind() const noexcept {
        return CardFilterKind::Unknown;
    };
};

/// @brief Card filter chain (helper struct)
struct CardFilterChain final : public CardSearchFilter {
    std::vector<std::unique_ptr<CardSearchFilter>> chain;

    CardFilterChain() = default;

    template <typename Filter>
        requires(!std::is_same_v<CardFilterChain, std::decay_t<Filter>>)
    CardFilterChain(Filter&& filter) {
        chain.push_back(std::make_unique<std::decay_t<Filter>>(std::forward<Filter>(filter)));
    }

    template <typename Filter>
        requires std::is_base_of_v<CardSearchFilter, std::decay_t<Filter>>
    CardFilterChain& add(Filter&& filter) {
        chain.push_back(std::make_unique<std::decay_t<Filter>>(std::forward<Filter>(filter)));
        return *this;
    }
};

template <typename Filter>
concept CardFilter =
    std::is_base_of_v<CardSearchFilter, std::decay_t<Filter>> && !std::is_same_v<CardFilterChain, std::decay_t<Filter>>;

template <CardFilter L, CardFilter R> CardFilterChain operator|(L&& lhs, R&& rhs) {
    CardFilterChain result;

    result.add(std::forward<L>(lhs));
    result.add(std::forward<R>(rhs));

    return result;
}

template <CardFilter Filter> CardFilterChain operator|(CardFilterChain lhs, Filter&& rhs) {
    lhs.add(std::forward<Filter>(rhs));
    return lhs;
}

/// @brief Filter search results by deck ID
struct CardDeckIdSearchFilter final : public CardSearchFilter {
    CardDeckIdSearchFilter(int deck_id) : deck_id{deck_id} {}

    CardFilterKind kind() const noexcept override {
        return CardFilterKind::DeckId;
    }

    int deck_id{0};
};

/// @brief Filter search results by the front side of the card
struct CardFrontSearchFilter final : public CardSearchFilter {
    CardFrontSearchFilter(QString front) : front{std::move(front)} {}

    CardFilterKind kind() const noexcept override {
        return CardFilterKind::Front;
    }

    QString front;
};

} // namespace core