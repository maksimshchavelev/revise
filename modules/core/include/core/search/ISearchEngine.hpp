// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "CardSearchFilter.hpp" // for card filters
#include "core/Card.hpp"        // for Card
#include <expected>             // for std::expected
#include <system_error>         // for std::error_code

namespace core {

/// @brief A structure describing a search error for the `ISearchEngine` class
struct SearchError {
    enum class Kind { InvalidFilter, BackendFailure, Unknown };

    Kind            kind{Kind::Unknown};
    std::error_code cause;
    QString         message;
};

/**
 * @brief Search engine interface. Used to search for cards and decks
 *
 * The implementation must inherit from and implement purely virtual methods.
 *
 * Note: A specific filter can be identified by iterating through the filter chain and performing a `dynamic_cast` on
 * each filter type.
 */
class ISearchEngine {
  public:
    virtual ~ISearchEngine() = default;

    /**
     * @brief Searching for cards
     *
     * Example usage:
     * @code{.cpp}
     * auto res = search_engine.search_cards(CardDeckIdSearchFilter{45} | CardFrontSearchFilter{"front"});
     * @endcode
     *
     * @param filters Filters
     */
    virtual std::expected<QVector<Card>, SearchError> search_cards(const CardFilterChain& filters) const = 0;
};

} // namespace core