// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <TestDatabase.hpp>                // for TestDatabase helper
#include <core/structs/Card.hpp>           // for core::Card
#include <core/structs/Deck.hpp>           // for core::Deck
#include <gtest/gtest.h>                   // for gtest
#include <io/DatabaseExecutionContext.hpp> // for io::DatabaseExecutionContext
#include <io/SqlDeckStorage.hpp>           // for io::SqlDeckStorage

using namespace io;

// Verifies that a newly created storage returns empty vectors for decks and cards.
TEST(sql_deck_storage, empty_storage_returns_empty_vectors) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    const auto storage = SqlDeckStorage{database.database, context};

    const auto decks = storage.fetch_decks();
    ASSERT_TRUE(decks);
    EXPECT_TRUE(decks->isEmpty());

    const auto cards = storage.fetch_cards();
    ASSERT_TRUE(cards);
    EXPECT_TRUE(cards->isEmpty());
}


// Ensures that multiple decks can be created and correctly fetched by both all and specific IDs.
TEST(sql_deck_storage, create_and_fetch_decks) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const QVector<core::Deck> decks_to_create = {{"Deck 1", "Description 1", 0, 0, 10, 20, 30, 40},
                                                 {"Deck 2", "Description 2", 0, 0, 5, 15, 25, 35}};

    const auto created_decks = storage.create_decks(decks_to_create);
    ASSERT_TRUE(created_decks);
    ASSERT_EQ(created_decks->size(), 2);

    EXPECT_NE((*created_decks)[0].id, 0);
    EXPECT_NE((*created_decks)[1].id, 0);

    const auto all_decks = storage.fetch_decks();
    ASSERT_TRUE(all_decks);
    EXPECT_EQ(all_decks->size(), 2);

    const QVector<core::Deck::id_type> ids = {(*created_decks)[0].id};
    const auto                         specific_decks = storage.fetch_decks(ids);
    ASSERT_TRUE(specific_decks);
    EXPECT_EQ(specific_decks->size(), 1);
    EXPECT_EQ((*specific_decks)[0].name, "Deck 1");
}


// Checks that an existing deck can be updated with new data and correctly fetched.
TEST(sql_deck_storage, update_deck) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Old Name", "Old Desc", 0, 0, 10, 20, 30, 40}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    core::Deck deck_to_update = (*created_decks)[0];
    deck_to_update.name = "New Name";
    deck_to_update.description = "New Desc";
    deck_to_update.time_limit = 99;

    const auto updated_decks = storage.update_decks(QVector<core::Deck>{deck_to_update});
    ASSERT_TRUE(updated_decks);
    EXPECT_EQ((*updated_decks)[0].name, "New Name");

    const auto fetched_decks = storage.fetch_decks(QVector<core::Deck::id_type>{deck_id});
    ASSERT_TRUE(fetched_decks);
    EXPECT_EQ((*fetched_decks)[0].name, "New Name");
    EXPECT_EQ((*fetched_decks)[0].time_limit, 99);
}


// Verifies that a deck can be removed and is no longer present in the storage.
TEST(sql_deck_storage, remove_decks) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Deck 1", "Description 1", 0, 0, 10, 20, 30, 40}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    const auto removed = storage.remove_decks(QVector<core::Deck::id_type>{deck_id});
    ASSERT_TRUE(removed);
    EXPECT_EQ(removed->size(), 1);

    const auto all_decks = storage.fetch_decks();
    ASSERT_TRUE(all_decks);
    EXPECT_TRUE(all_decks->isEmpty());
}


// Ensures that multiple cards can be created and correctly fetched by deck ID, card IDs, and globally.
TEST(sql_deck_storage, create_and_fetch_cards) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Test Deck", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    const QVector<core::Card> cards_to_create = {{0,
                                                  0,
                                                  deck_id,
                                                  2.5f,
                                                  core::Card::State::New,
                                                  0,
                                                  0,
                                                  0,
                                                  QDateTime::currentDateTime(),
                                                  QDateTime::currentDateTime(),
                                                  QDateTime::currentDateTime(),
                                                  "Front 1",
                                                  "Back 1"},
                                                 {0,
                                                  0,
                                                  deck_id,
                                                  1.0f,
                                                  core::Card::State::Review,
                                                  0,
                                                  0,
                                                  0,
                                                  QDateTime::currentDateTime(),
                                                  QDateTime::currentDateTime(),
                                                  QDateTime::currentDateTime(),
                                                  "Front 2",
                                                  "Back 2"}};

    const auto created_cards = storage.create_cards(cards_to_create);
    ASSERT_TRUE(created_cards);
    ASSERT_EQ(created_cards->size(), 2);

    const auto card1_id = (*created_cards)[0].id;
    const auto card2_id = (*created_cards)[1].id;
    ASSERT_NE(card1_id, 0);
    ASSERT_NE(card2_id, 0);

    const auto cards_by_deck = storage.fetch_cards(deck_id);
    ASSERT_TRUE(cards_by_deck);
    EXPECT_EQ(cards_by_deck->size(), 2);

    const auto cards_by_id = storage.fetch_cards(QVector<core::Card::id_type>{card1_id});
    ASSERT_TRUE(cards_by_id);
    EXPECT_EQ(cards_by_id->size(), 1);
    EXPECT_EQ((*cards_by_id)[0].front, "Front 1");

    const auto all_cards = storage.fetch_cards();
    ASSERT_TRUE(all_cards);
    EXPECT_EQ(all_cards->size(), 2);
}


// Checks that an existing card can be updated and the changes are correctly saved.
TEST(sql_deck_storage, update_card) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Test Deck", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    const auto created_cards = storage.create_cards(QVector<core::Card>{{0,
                                                                         0,
                                                                         deck_id,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "Front 1",
                                                                         "Back 1"}});
    ASSERT_TRUE(created_cards);
    const auto card_id = (*created_cards)[0].id;

    core::Card card_to_update = (*created_cards)[0];
    card_to_update.front = "Updated Front";
    card_to_update.difficulty = 4.5f;
    card_to_update.state = core::Card::State::Incorrect;

    const auto updated_cards = storage.update_cards(QVector<core::Card>{card_to_update});
    ASSERT_TRUE(updated_cards);

    const auto fetched_cards = storage.fetch_cards(QVector<core::Card::id_type>{card_id});
    ASSERT_TRUE(fetched_cards);
    EXPECT_EQ((*fetched_cards)[0].front, "Updated Front");
    EXPECT_FLOAT_EQ((*fetched_cards)[0].difficulty, 4.5f);
    EXPECT_EQ((*fetched_cards)[0].state, core::Card::State::Incorrect);
}


// Verifies that a card can be removed from the storage by its ID.
TEST(sql_deck_storage, remove_cards) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Test Deck", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    const auto created_cards = storage.create_cards(QVector<core::Card>{{0,
                                                                         0,
                                                                         deck_id,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "Front 1",
                                                                         "Back 1"}});
    ASSERT_TRUE(created_cards);
    const auto card_id = (*created_cards)[0].id;

    const auto removed_cards = storage.remove_cards(QVector<core::Card::id_type>{card_id});
    ASSERT_TRUE(removed_cards);
    EXPECT_EQ(removed_cards->size(), 1);

    const auto all_cards = storage.fetch_cards(deck_id);
    ASSERT_TRUE(all_cards);
    EXPECT_TRUE(all_cards->isEmpty());
}


// Ensures that attempting to create an empty list of decks returns an empty result without error.
TEST(sql_deck_storage, create_decks_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.create_decks(QVector<core::Deck>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that attempting to update an empty list of decks returns an empty result without error.
TEST(sql_deck_storage, update_decks_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.update_decks(QVector<core::Deck>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that attempting to remove an empty list of deck IDs returns an empty result without error.
TEST(sql_deck_storage, remove_decks_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.remove_decks(QVector<core::Deck::id_type>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that fetching decks with an empty list of IDs returns an empty result.
TEST(sql_deck_storage, fetch_decks_with_empty_ids_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    const auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.fetch_decks(QVector<core::Deck::id_type>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Verifies that fetching decks with non-existent IDs returns an empty result or a NotFound error.
TEST(sql_deck_storage, fetch_decks_with_non_existent_ids_returns_empty_or_not_found) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    const auto storage = SqlDeckStorage{database.database, context};

    if (const auto result = storage.fetch_decks(QVector<core::Deck::id_type>{999, 1000})) {
        EXPECT_TRUE(result->isEmpty());
    } else {
        EXPECT_EQ(result.error().kind, core::IDeckStorage::ErrorKind::NotFound);
    }
}


// Checks that updating a non-existent deck returns NotFound or an empty result.
TEST(sql_deck_storage, update_non_existent_deck_returns_not_found_or_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const core::Deck non_existent{"Ghost Deck", "", 999, 0, 0, 0, 0, 0};

    if (const auto result = storage.update_decks(QVector<core::Deck>{non_existent})) {
        EXPECT_TRUE(result->isEmpty());
    } else {
        EXPECT_EQ(result.error().kind, core::IDeckStorage::ErrorKind::NotFound);
    }
}


// Checks that removing a non-existent deck safely returns NotFound or an empty result.
TEST(sql_deck_storage, remove_non_existent_deck_is_safe) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    if (const auto result = storage.remove_decks(QVector<core::Deck::id_type>{999})) {
        EXPECT_TRUE(result->isEmpty());
    } else {
        EXPECT_EQ(result.error().kind, core::IDeckStorage::ErrorKind::NotFound);
    }
}


// Verifies that creating a deck with empty name and description fields is allowed.
TEST(sql_deck_storage, create_deck_with_empty_strings_is_allowed) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.create_decks(QVector<core::Deck>{{"", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(result);
    ASSERT_EQ(result->size(), 1);
    EXPECT_TRUE((*result)[0].name.isEmpty());
}


// Ensures that attempting to create an empty list of cards returns an empty result without error.
TEST(sql_deck_storage, create_cards_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.create_cards(QVector<core::Card>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that attempting to update an empty list of cards returns an empty result without error.
TEST(sql_deck_storage, update_cards_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.update_cards(QVector<core::Card>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that attempting to remove an empty list of card IDs returns an empty result without error.
TEST(sql_deck_storage, remove_cards_with_empty_input_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.remove_cards(QVector<core::Card::id_type>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that fetching cards with an empty list of IDs returns an empty result.
TEST(sql_deck_storage, fetch_cards_with_empty_ids_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.fetch_cards(QVector<core::Card::id_type>{});
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Verifies that fetching cards for a non-existent deck ID returns an empty result.
TEST(sql_deck_storage, fetch_cards_for_non_existent_deck_returns_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    const auto storage = SqlDeckStorage{database.database, context};

    const auto result = storage.fetch_cards(999);
    ASSERT_TRUE(result);
    EXPECT_TRUE(result->isEmpty());
}


// Ensures that creating a card with an invalid deck ID returns an InvalidArgument error.
TEST(sql_deck_storage, create_card_with_invalid_deck_id_returns_error) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const core::Card invalid_card{0,
                                  0,
                                  999,
                                  2.5f,
                                  core::Card::State::New,
                                  0,
                                  0,
                                  0,
                                  QDateTime::currentDateTime(),
                                  QDateTime::currentDateTime(),
                                  QDateTime::currentDateTime(),
                                  "Front",
                                  "Back"};

    const auto result = storage.create_cards(QVector<core::Card>{invalid_card});
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::IDeckStorage::ErrorKind::InvalidArgument);
}


// Checks that updating a non-existent card returns NotFound or an empty result.
TEST(sql_deck_storage, update_non_existent_card_returns_not_found_or_empty) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const core::Card non_existent{999,
                                  0,
                                  1,
                                  2.5f,
                                  core::Card::State::New,
                                  0,
                                  0,
                                  0,
                                  QDateTime::currentDateTime(),
                                  QDateTime::currentDateTime(),
                                  QDateTime::currentDateTime(),
                                  "Ghost",
                                  "Ghost"};

    if (const auto result = storage.update_cards(QVector<core::Card>{non_existent})) {
        EXPECT_TRUE(result->isEmpty());
    } else {
        EXPECT_EQ(result.error().kind, core::IDeckStorage::ErrorKind::NotFound);
    }
}


// Verifies that fetching cards with a mix of valid and invalid IDs returns only the existing cards.
TEST(sql_deck_storage, fetch_cards_with_mixed_existing_and_non_existing_ids) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto created_decks = storage.create_decks(QVector<core::Deck>{{"Test Deck", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(created_decks);
    const auto deck_id = (*created_decks)[0].id;

    const auto created_cards = storage.create_cards(QVector<core::Card>{{0,
                                                                         0,
                                                                         deck_id,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "Front 1",
                                                                         "Back 1"}});
    ASSERT_TRUE(created_cards);
    const auto valid_id = (*created_cards)[0].id;

    const auto result = storage.fetch_cards(QVector<core::Card::id_type>{valid_id, 999});
    ASSERT_TRUE(result);
    EXPECT_EQ(result->size(), 1);
    EXPECT_EQ((*result)[0].id, valid_id);
}


// Ensures that fetching all cards returns cards from all decks in the storage.
TEST(sql_deck_storage, fetch_all_cards_returns_cards_from_multiple_decks) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto decks =
        storage.create_decks(QVector<core::Deck>{{"Deck A", "", 0, 0, 0, 0, 0, 0}, {"Deck B", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(decks);
    const auto deck_a = (*decks)[0].id;
    const auto deck_b = (*decks)[1].id;

    const auto created_cards = storage.create_cards(QVector<core::Card>{{0,
                                                                         0,
                                                                         deck_a,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "A1",
                                                                         "B1"},
                                                                        {0,
                                                                         0,
                                                                         deck_b,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "A2",
                                                                         "B2"}});
    ASSERT_TRUE(created_cards);

    const auto result = storage.fetch_cards();
    ASSERT_TRUE(result);
    EXPECT_EQ(result->size(), 2);
}


// Verifies that removing a deck with cards either cascades (deletes cards) or returns an error.
TEST(sql_deck_storage, removing_deck_cascades_or_fails_gracefully_if_has_cards) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto decks = storage.create_decks(QVector<core::Deck>{{"Deck to delete", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(decks);
    const auto deck_id = (*decks)[0].id;

    const auto created_cards = storage.create_cards(QVector<core::Card>{{0,
                                                                         0,
                                                                         deck_id,
                                                                         2.5f,
                                                                         core::Card::State::New,
                                                                         0,
                                                                         0,
                                                                         0,
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         QDateTime::currentDateTime(),
                                                                         "Front",
                                                                         "Back"}});
    ASSERT_TRUE(created_cards);

    if (const auto remove_result = storage.remove_decks(QVector<core::Deck::id_type>{deck_id})) {
        const auto remaining_cards = storage.fetch_cards(deck_id);
        ASSERT_TRUE(remaining_cards);
        EXPECT_TRUE(remaining_cards->isEmpty()) << "Cards were not deleted after deck removal (no cascade)";
    } else {
        EXPECT_EQ(remove_result.error().kind, core::IDeckStorage::ErrorKind::InvalidArgument);
    }
}


// Verifies that the storage correctly handles Unicode characters, emojis, and very long strings.
TEST(sql_deck_storage, handles_unicode_and_long_strings_correctly) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const QString unicode_name = "Колода 🚀 с эмодзи и юникодом";
    const QString long_description(1000, 'A');

    const auto decks = storage.create_decks(QVector<core::Deck>{{unicode_name, long_description, 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(decks);
    const auto deck_id = (*decks)[0].id;

    const auto fetched = storage.fetch_decks(QVector<core::Deck::id_type>{deck_id});
    ASSERT_TRUE(fetched);
    EXPECT_EQ((*fetched)[0].name, unicode_name);
    EXPECT_EQ((*fetched)[0].description, long_description);
}


// Ensures that a card with empty string fields and default dates can be saved and retrieved correctly.
TEST(sql_deck_storage, card_with_empty_fields_is_saved_and_fetched) {
    auto                     database = helpers::create_database();
    DatabaseExecutionContext context{};

    ASSERT_TRUE(database.database.init_db());

    auto storage = SqlDeckStorage{database.database, context};

    const auto decks = storage.create_decks(QVector<core::Deck>{{"Deck", "", 0, 0, 0, 0, 0, 0}});
    ASSERT_TRUE(decks);
    const auto deck_id = (*decks)[0].id;

    const core::Card empty_card{
        0, 0, deck_id, 0.0f, core::Card::State::New, 0, 0, 0, QDateTime(), QDateTime(), QDateTime(), "", ""};

    const auto created = storage.create_cards(QVector<core::Card>{empty_card});
    ASSERT_TRUE(created);
    const auto card_id = (*created)[0].id;

    const auto fetched = storage.fetch_cards(QVector<core::Card::id_type>{card_id});
    ASSERT_TRUE(fetched);
    EXPECT_TRUE((*fetched)[0].front.isEmpty());
    EXPECT_TRUE((*fetched)[0].back.isEmpty());
}
