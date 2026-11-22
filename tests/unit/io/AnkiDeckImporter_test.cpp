// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>              // for gtest
#include <io/DeckImporterFactory.hpp> // for create_deck_importer

using namespace core;
using namespace io;


class AnkiDeckImporterTest : public testing::Test {
  protected:
    const std::unique_ptr<IDeckImporter> importer{create_deck_importer(ImporterFormat::ANKI)};
};


TEST_F(AnkiDeckImporterTest, anki_format_name) {
    EXPECT_EQ(importer->format_name(), "anki");
}


TEST_F(AnkiDeckImporterTest, import_deck_with_cards) {
    auto result = importer->import_from_file("./testdata/deck.apkg");

    ASSERT_TRUE(result);

    EXPECT_EQ(result->deck_name, "deck");
    EXPECT_EQ(result->deck_description, QString{});
    EXPECT_EQ(result->time_limit, 0);       // default value
    EXPECT_EQ(result->new_limit, 30);       // default value
    EXPECT_EQ(result->review_limit, 30);    // default value
    EXPECT_EQ(result->incorrect_limit, 30); // default value

    // ------------------ cards ------------------

    std::ranges::sort(result->cards, {}, &Card::front);

    qDebug() << result->cards[0].back;

    EXPECT_EQ(result->cards[0].front, "Card 1");
    EXPECT_EQ(result->cards[0].back, "Back of card 1");

    EXPECT_EQ(result->cards[1].front, "Card 2");
    EXPECT_EQ(result->cards[1].back, "Back of card 2");
}


TEST_F(AnkiDeckImporterTest, cant_import_broken_file) {
    const auto result = importer->import_from_file("./testdata/broken.apkg");

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, IDeckImporter::ErrorKind::ExtractionFailed);
}
