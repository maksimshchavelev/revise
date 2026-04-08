#include <core/search/CardSearchFilter.hpp>
#include <gtest/gtest.h>

using namespace core;

TEST(CardSearchFilter, filter_chain_3) {
    const CardFilterChain filter =
        CardDeckIdSearchFilter{45} | CardFrontSearchFilter{"front"} | CardDeckIdSearchFilter{20};
    const auto& chain = filter.chain;

    ASSERT_EQ(chain.size(), 3);

    const CardDeckIdSearchFilter* filter1 = dynamic_cast<CardDeckIdSearchFilter*>(chain[0].get());
    const CardFrontSearchFilter*  filter2 = dynamic_cast<CardFrontSearchFilter*>(chain[1].get());
    const CardDeckIdSearchFilter* filter3 = dynamic_cast<CardDeckIdSearchFilter*>(chain[2].get());

    ASSERT_TRUE(filter1);
    ASSERT_TRUE(filter2);
    ASSERT_TRUE(filter3);

    ASSERT_EQ(filter1->deck_id, 45);
    ASSERT_STREQ(filter2->front.toStdString().c_str(), "front");
    ASSERT_EQ(filter3->deck_id, 20);
}


TEST(CardSearchFilter, filter_chain_2) {
    const CardFilterChain filter = CardDeckIdSearchFilter{45} | CardFrontSearchFilter{"front"};
    const auto&           chain = filter.chain;

    ASSERT_EQ(chain.size(), 2);

    const CardDeckIdSearchFilter* filter1 = dynamic_cast<CardDeckIdSearchFilter*>(chain[0].get());
    const CardFrontSearchFilter*  filter2 = dynamic_cast<CardFrontSearchFilter*>(chain[1].get());

    ASSERT_TRUE(filter1);
    ASSERT_TRUE(filter2);

    ASSERT_EQ(filter1->deck_id, 45);
    ASSERT_STREQ(filter2->front.toStdString().c_str(), "front");
}


TEST(CardSearchFilter, filter_chain_1) {
    const CardFilterChain filter = CardDeckIdSearchFilter{45};
    const auto&           chain = filter.chain;

    ASSERT_EQ(chain.size(), 1);

    const CardDeckIdSearchFilter* filter1 = dynamic_cast<CardDeckIdSearchFilter*>(chain[0].get());

    ASSERT_TRUE(filter1);

    ASSERT_EQ(filter1->deck_id, 45);
}


TEST(CardSearchFilter, filter_chain_empty) {
    const CardFilterChain filter;

    ASSERT_TRUE(filter.chain.empty());
}


TEST(CardSearchFilter, filter_chain_move) {
    CardFilterChain filter1 = CardDeckIdSearchFilter{45};
    CardFilterChain filter2 = std::move(filter1);

    ASSERT_TRUE(filter1.chain.empty());
    ASSERT_FALSE(filter2.chain.empty());
}

