// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <TestSqlDeckStorage.hpp>  // for helpers::create_storage()
#include <engine/SearchEngine.hpp> // for SearchEngine
#include <gtest/gtest.h>           // for gtest
#include <io/SqlDeckStorage.hpp>   // for SqlDeckStorage

using namespace engine;
using namespace io;
using namespace core;


class SearchEngineTest : public testing::Test {
  protected:
    void SetUp() override {
        if (const auto res = storage.storage.create_decks({decks[0]}); !res) {
            FAIL();
        } else {
            cards[0].deck_id = res->first().id;
        }

        if (const auto res = storage.storage.create_decks({decks[1]}); !res) {
            FAIL();
        } else {
            cards[1].deck_id = res->first().id;
        }

        if (const auto res = storage.storage.create_cards(cards); !res) {
            FAIL();
        } else {
            cards = std::move(*res);
        }
    }

    helpers::TestSqlDeckStorage storage{helpers::create_storage()};
    SearchEngine                engine{storage.storage};

    QVector<Card> cards = {// ------------------------- Card 1 -------------------------
                           {0,
                            0,
                            1,
                            3.4f,
                            Card::State::New,
                            0,
                            0,
                            10,
                            QDateTime::currentDateTime(),
                            QDateTime::currentDateTime(),
                            QDateTime::currentDateTime(),
                            "SoMe CaRd",
                            "Back of the some card"},
                           // ------------------------- Card 2 -------------------------
                           {0,
                            0,
                            2,
                            3.4f,
                            Card::State::New,
                            0,
                            0,
                            10,
                            QDateTime::currentDateTime(),
                            QDateTime::currentDateTime(),
                            QDateTime::currentDateTime(),
                            R"(\(\left(\arcsin(\sqrt{x}^2)\right)\))",
                            "Back of the some card with KaTeX front"}};

    const QVector<Deck> decks = {// ------------------------- Deck 1 -------------------------
                                 {"Deck 1", "A deck 1 with description", 0, 10, 20, 30, 40},
                                 // ------------------------- Deck 2 -------------------------
                                 {"Deck 2", "A deck 2 with description", 0, 20, 30, 40, 50}};
};


// Select all cards in specific deck. Only Deck ID filter is applied
TEST_F(SearchEngineTest, find_cards_by_deck_id) {
    const auto found = engine.search_cards(CardDeckIdSearchFilter{1});

    ASSERT_TRUE(found);
    EXPECT_EQ(*found, QVector{cards[0]});
}


// Select all cards if no filters provided
TEST_F(SearchEngineTest, no_card_filters_provived) {
    const auto found = engine.search_cards({});

    ASSERT_TRUE(found);
    EXPECT_EQ(*found, cards);
}


// No cards found if not-existing deck id was provided
TEST_F(SearchEngineTest, found_cards_from_not_existing_deck) {
    const auto found = engine.search_cards(CardDeckIdSearchFilter{999});

    ASSERT_TRUE(found);
    EXPECT_TRUE(found->empty());
}


// Case-insensitive search anywhere on the front of the card
TEST_F(SearchEngineTest, case_insensetive_search_anywhere_on_the_front_of_the_card) {
    const auto found = engine.search_cards(CardFrontSearchFilter{"me ca"});

    ASSERT_TRUE(found);
    EXPECT_EQ(*found, QVector{cards[0]});
}


// When searching for KaTeX, the symbols are converted to equivalent human-readable characters
TEST_F(SearchEngineTest, katex_replacement_when_found_by_front_of_the_card) {
    const auto found = engine.search_cards(CardFrontSearchFilter{"(arcsin(√{x}^2))"});

    ASSERT_TRUE(found);
    EXPECT_EQ(*found, QVector{cards[1]});
}


// Filters combination
TEST_F(SearchEngineTest, card_filters_combinations) {
    const auto found = engine.search_cards(CardFrontSearchFilter{"me ca"} | CardDeckIdSearchFilter{1});

    ASSERT_TRUE(found);
    EXPECT_EQ(*found, QVector{cards[0]});
}
