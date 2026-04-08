// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckStorage.hpp>         // for core::IDeckStorage
#include <core/search/ISearchEngine.hpp> // for core::ISearchEngine

namespace engine {

class SearchEngine final : public core::ISearchEngine {
  public:
    SearchEngine(core::IDeckStorage& storage);

    /**
     * @brief Searching for records in an SQL database
     * @param filters Filters
     * @note When searching using “front/back,” the search is performed using fuzzy matching
     */
    std::expected<QVector<core::Card>, core::SearchError> search_cards(
        const core::CardFilterChain& filters) const override;

  private:
    core::IDeckStorage& m_storage;

    /// @brief Returns true if the card matches at least one type
    bool card_matches(const core::Card& card, const std::unique_ptr<core::CardSearchFilter>& filter) const;
};

} // namespace engine