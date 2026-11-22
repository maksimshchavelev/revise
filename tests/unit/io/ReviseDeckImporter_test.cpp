// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>              // for gtest
#include <io/DeckImporterFactory.hpp> // for create_deck_importer

using namespace core;
using namespace io;


class ReviseDeckImporterTest : public testing::Test {
  protected:
    const std::unique_ptr<IDeckImporter> importer{create_deck_importer(ImporterFormat::REVISE)};
};


TEST_F(ReviseDeckImporterTest, revise_format_name) {
    EXPECT_EQ(importer->format_name(), "revise");
}


TEST_F(ReviseDeckImporterTest, import_deck_with_cards) {
    auto result = importer->import_from_file("./testdata/deck.rpkg");

    ASSERT_TRUE(result);

    EXPECT_EQ(result->deck_name, "deck");
    EXPECT_EQ(result->deck_description, "description");
    EXPECT_EQ(result->time_limit, 1);
    EXPECT_EQ(result->new_limit, 2);
    EXPECT_EQ(result->review_limit, 3);
    EXPECT_EQ(result->incorrect_limit, 4);

    // ------------------ cards ------------------

    std::ranges::sort(result->cards, {}, &Card::front);

    EXPECT_EQ(result->cards[0].front, "<p>Card 1</p>");
    EXPECT_EQ(result->cards[0].back, "<p>Back of card 1</p>");

    EXPECT_EQ(result->cards[1].front, "<p>Card 2</p>");
    EXPECT_EQ(result->cards[1].back, "<p>Back of card 2</p>");
}


TEST_F(ReviseDeckImporterTest, cant_import_broken_file) {
    const auto result = importer->import_from_file("./testdata/broken.rpkg");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, IDeckImporter::ErrorKind::ExtractionFailed);
}

