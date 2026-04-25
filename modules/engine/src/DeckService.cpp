// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/DeckService.hpp" // for header
#include <QFileInfo>              // for QFileInfo
#include <QFuture>                // for QFuture
#include <QRegularExpression>     // for QRegularExpression
#include <QUrl>                   // for QUrl
#include <QtConcurrentRun>        // for QtConcurrent::run
#include <QtGlobal>               // for math
#include <QtMinMax>               // for min / max
#include <utils/Html.hpp>         // for html utils
#include <utils/ScopeGuard.hpp>   // for ScopeGuard

namespace engine {

DeckService::DeckService(DeckServiceDeps deps) : m_deps(deps) {}


QFuture<core::IDeckService::Result<void>> DeckService::create_deck(const DeckDraft& deck) {
    return QtConcurrent::run([this, deck]() -> core::IDeckService::Result<void> {
        if (deck.name.isEmpty()) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck name is empty");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (deck.description.isEmpty()) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck description is empty");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (deck.new_limit < 0) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck new limit is less than 0");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (deck.review_limit < 0) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck review limit is less than 0");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (deck.incorrect_limit < 0) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck incorrect limit is less than 0");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (deck.time_limit < 0) {
            auto error = Error::validation_failed("DeckService::create_deck(): deck time limit is less than 0");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        core::Deck new_deck{.name = deck.name,
                            .description = deck.description,
                            .time_limit = deck.time_limit,
                            .new_limit = deck.new_limit,
                            .review_limit = deck.review_limit,
                            .incorrect_limit = deck.incorrect_limit};

        auto create_res = m_deps.deck_storage.create_decks({new_deck});

        if (!create_res) {
            auto error = from_storage_error(create_res.error(), "DeckService::create_deck(): failed to create deck");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        if (create_res->empty()) {
            auto error =
                Error::internal_error("DeckService::create_deck(): after creation, deck storage did not return the created deck");

            dispatch(deck_create_failed{error});
            return std::unexpected(error);
        }

        dispatch(deck_created{create_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::remove_deck(core::Deck::id_type deck_id) {
    return QtConcurrent::run([this, deck_id]() -> core::IDeckService::Result<void> {
        auto remove_res = m_deps.deck_storage.remove_decks({deck_id});

        if (!remove_res) {
            auto error = from_storage_error(remove_res.error(), "DeckService::remove_deck(): failed to remove deck");

            dispatch(deck_remove_failed{error});
            return std::unexpected(error);
        }

        if (!remove_res->empty()) {
            auto error =
                Error::internal_error("DeckService::create_deck(): after removing, deck storage did not return the removed deck");

            dispatch(deck_remove_failed{error});
            return std::unexpected(error);
        }

        m_deps.deck_media_storage.remove_deck_media(deck_id);

        dispatch(deck_removed{remove_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::import_deck(const QString& path) {
    return QtConcurrent::run([this, path]() -> core::IDeckService::Result<void> {
        if (m_import_in_progress.load()) {
            return {};
        }

        utils::ScopeGuard guard(
            [this]() { // on enter
                m_import_in_progress.store(true);
            },
            [this]() { // on exit
                m_import_in_progress.store(false);
                dispatch(import_finished{});
            });

        const QUrl    url(path);
        const QString local_path = url.isLocalFile() ? url.toLocalFile() : url.toString();

        auto extract_extension = [](const QString& filename) -> QString {
            // Matches ".apkg", ".zip", ".anki" etc. before optional " (n)"
            static const QRegularExpression re(R"(\.([A-Za-z0-9]+)(?:\s*\(\d+\))?$)");

            auto match = re.match(filename);
            if (!match.hasMatch()) {
                return {};
            }

            return match.captured(1).toLower();
        };

        std::expected<core::ImportResult, QString> import_res;

        // Select importer automatically
        if (auto suffix = extract_extension(QFileInfo(local_path).fileName()); suffix == "apkg") {
            import_res = m_deps.anki_deck_importer.import_from_file(local_path);
        } else if (suffix == "rpkg") {
            import_res = m_deps.revise_deck_importer.import_from_file(local_path);
        } else {
            auto error = Error::unsupported_format(
                QString("DeckService::import_deck(): Unknown file extension: %1 (cannot select importer for file %2)")
                    .arg(suffix)
                    .arg(local_path));

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        QVector<core::Card> cards_to_update;
        QVector<core::Card> cards_to_insert;

        if (!import_res) {
            auto error = Error::internal_error(QString("DeckService::import_deck(): importer error: %1").arg(import_res.error()));

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        // Check if a deck with that name exists
        auto deck_fetch_request = m_deps.deck_storage.fetch_decks();

        if (!deck_fetch_request) {
            auto error = Error::internal_error(
                QString("DeckService::import_deck(): could not verify the existence of a deck with the name: %1")
                    .arg(import_res->deck_name));

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        auto exists_decks_filter = *deck_fetch_request | std::views::filter([&import_res](const auto& deck) {
            return deck.name == import_res->deck_name;
        });

        QVector<core::Deck> exists_decks(exists_decks_filter.begin(), exists_decks_filter.end());

        bool deck_exists = exists_decks.size() > 0;
        int  deck_id{0};

        core::Deck deck;

        // Get ID if deck exists
        if (deck_exists) {
            deck_id = deck_fetch_request->first().id;
            deck = deck_fetch_request->first();
        } else {
            // else create new deck
            auto res = m_deps.deck_storage.create_decks({core::Deck{.name = import_res->deck_name,
                                                                    .description = import_res->deck_description,
                                                                    .time_limit = 0,
                                                                    .new_limit = 30,
                                                                    .review_limit = 30,
                                                                    .incorrect_limit = 30}});

            if (!res) {
                auto error = from_storage_error(res.error(), "DeckService::import_deck(): failed to create new deck");

                dispatch(import_failed{error});
                return std::unexpected(error);
            }

            deck_id = res->first().id;
            deck = res->first();
        }

        // Update deck parameters
        if (auto res = m_deps.deck_storage.update_decks({core::Deck{.name = import_res->deck_name,
                                                                    .description = import_res->deck_description,
                                                                    .id = deck_id,
                                                                    .time_limit = import_res->time_limit,
                                                                    .new_limit = import_res->new_limit,
                                                                    .review_limit = import_res->consolidate_limit,
                                                                    .incorrect_limit = import_res->incorrect_limit}});
            !res) {
            auto error = from_storage_error(res.error(), "DeckService::import_deck(): failed to update deck");

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        // Save all images
        QMap<QString, QByteArray> images = import_res->user_data.value<QMap<QString, QByteArray>>();
        QMap<QString /* image name */, QString /* path to image */> mapped_images;

        for (const auto& [name, data] : images.toStdMap()) {
            if (auto res = m_deps.deck_media_storage.save_image(deck_id, name, data); !res) {
                auto error = Error::internal_error(
                    QString("DeckService::import_deck(): Failed to save deck image with name %1 (deck id: %2), cause: %3")
                        .arg(name)
                        .arg(deck_id)
                        .arg(res.error()));

                dispatch(import_failed{error});
                return std::unexpected(error);
            } else {
                mapped_images[name] = "file://" + res.value();
            }
        }

        // Update img sources
        for (auto& imported_card : import_res->cards) {
            imported_card.back = utils::Html::replace_images_src(imported_card.back, mapped_images);
        }

        // Update cards if deck exists
        if (deck_exists) {
            auto cards = m_deps.deck_storage.fetch_cards(deck_id);

            // error
            if (!cards) {
                auto error = from_storage_error(cards.error(), "DeckService::import_deck(): failed to fetch cards");

                dispatch(import_failed{error});
                return std::unexpected(error);
            }

            // update cards
            for (const auto& card : import_res->cards) {
                auto iter = std::find_if(
                    cards.value().begin(), cards.value().end(), [&card](const core::Card& i) { return card.front == i.front; });

                // if card found in database
                if (iter != cards.value().end()) {
                    // update card and save to vector
                    core::Card card_found = std::move(*iter);

                    card_found.back = card.back;
                    card_found.front = card.front;
                    card_found.updated_at = QDateTime::currentDateTime();

                    cards_to_update.push_back(std::move(card_found));
                } else {
                    // insert card to db
                    core::Card new_card = std::move(card);

                    new_card.deck_id = deck_id;

                    cards_to_insert.push_back(std::move(new_card));
                }
            }
        } else {
            cards_to_insert = std::move(import_res->cards);

            for (auto& card : cards_to_insert) {
                card.deck_id = deck_id;
            }
        }

        // Update old
        if (auto res = m_deps.deck_storage.update_cards(cards_to_update); !res) {
            auto error = from_storage_error(res.error(), "DeckService::import_deck(): failed to update cards");

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        // Insert new
        if (auto res = m_deps.deck_storage.create_cards(cards_to_insert); !res) {
            auto error = from_storage_error(res.error(), "DeckService::import_deck(): failed to create cards");

            dispatch(import_failed{error});
            return std::unexpected(error);
        }

        dispatch(deck_imported{deck});
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::export_deck(core::Deck::id_type deck_id, const QString& path) {
    return QtConcurrent::run([this, deck_id, path]() -> core::IDeckService::Result<void> {
        if (m_export_in_progress.load()) {
            return {};
        }

        utils::ScopeGuard guard(
            [this]() { // on enter
                m_export_in_progress.store(true);
            },
            [this]() { // on exit
                m_export_in_progress.store(false);
                dispatch(export_finished{});
            });

        QUrl          url(path);
        const QString local_path = url.toLocalFile();

        core::Deck          deck;
        QVector<core::Card> cards;
        QString             media_dir;

        if (m_deps.deck_media_storage.has_media_directory(deck_id)) {
            media_dir = m_deps.deck_media_storage.deck_media_folder(deck_id);
        }

        // Fetch deck
        if (auto res = m_deps.deck_storage.fetch_decks(QVector<int>{deck_id}); res) {
            deck = std::move(res->first());
        } else {
            auto error = from_storage_error(res.error(), "DeckService::export_deck(): failed to fetch decks");

            dispatch(export_failed{error});
            return std::unexpected(error);
        }

        // Fetch card
        if (auto res = m_deps.deck_storage.fetch_cards(deck_id); res) {
            cards = std::move(*res);
        } else {
            auto error = from_storage_error(res.error(), "DeckService::export_deck(): failed to fetch cards");

            dispatch(export_failed{error});
            return std::unexpected(error);
        }

        auto export_res = m_deps.deck_exporter.export_to_file(
            core::ExportData{.deck = std::move(deck), .media_directory = std::move(media_dir), .cards = std::move(cards)},
            local_path);

        if (!export_res) {
            auto error =
                Error::internal_error(QString("DeckService::export_deck(): failed to export deck: %1").arg(export_res.error()));

            dispatch(export_failed{error});
            return std::unexpected(error);
        }

        dispatch(deck_exported{deck});
    });
}


QFuture<core::IDeckService::Result<core::Deck>> DeckService::deck(core::Deck::id_type deck_id) const {
    return QtConcurrent::run([this, deck_id]() -> core::IDeckService::Result<core::Deck> {
        auto res = m_deps.deck_storage.fetch_decks(QVector<int>{deck_id});

        if (!res) {
            auto error = from_storage_error(res.error(), "DeckService::deck(): failed to fetch deck");

            dispatch(deck_fetch_failed{error});
            return std::unexpected(error);
        }

        if (res->empty()) {
            auto error = Error::not_found("DeckService::deck(): deck not found");

            dispatch(deck_fetch_failed{error});
            return std::unexpected(error);
        }

        return res->first();
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::update_deck(core::Deck::id_type deck_id, const DeckDraft& deck) {
    return QtConcurrent::run([this, deck_id, deck]() -> core::IDeckService::Result<void> {
        if (deck.name.isEmpty()) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck name is empty");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (deck.description.isEmpty()) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck description is empty");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (deck.new_limit < 0) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck new limit is less than 0");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (deck.review_limit < 0) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck review limit is less than 0");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (deck.incorrect_limit < 0) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck incorrect limit is less than 0");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (deck.time_limit < 0) {
            auto error = Error::validation_failed("DeckService::update_deck(): deck time limit is less than 0");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        auto fetch_res = m_deps.deck_storage.fetch_decks({deck_id});

        if (!fetch_res) {
            auto error = from_storage_error(fetch_res.error(), "DeckService::update_deck(): failed to fetch deck");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (fetch_res->empty()) {
            auto error = Error::not_found("DeckService::update_deck(): deck not found");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        core::Deck fetched_deck = fetch_res->first();

        fetched_deck.name = deck.name;
        fetched_deck.description = deck.description;
        fetched_deck.new_limit = deck.new_limit;
        fetched_deck.review_limit = deck.review_limit;
        fetched_deck.incorrect_limit = deck.incorrect_limit;
        fetched_deck.time_limit = deck.time_limit;

        auto update_res = m_deps.deck_storage.update_decks({fetched_deck});

        if (!update_res) {
            auto error = from_storage_error(update_res.error(), "DeckService::update_deck(): failed to update deck");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        if (update_res->empty()) {
            auto error =
                Error::internal_error("DeckService::update_deck(): after updating, deck storage did not return the updated deck");

            dispatch(deck_update_failed{error});
            return std::unexpected(error);
        }

        dispatch(deck_updated{update_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::create_card(core::Deck::id_type deck_id, const CreateCardDraft& card) {
    return QtConcurrent::run([this, deck_id, card]() -> core::IDeckService::Result<void> {
        if (card.front.isEmpty()) {
            auto error = Error::validation_failed("DeckService::create_card(): card front is empty");

            dispatch(card_create_failed{error});
            return std::unexpected(error);
        }

        if (card.back.isEmpty()) {
            auto error = Error::validation_failed("DeckService::create_card(): card back is empty");

            dispatch(card_create_failed{error});
            return std::unexpected(error);
        }

        const auto now = QDateTime::currentDateTime();

        core::Card new_card;
        new_card.deck_id = deck_id;
        new_card.difficulty = 2.5f;
        new_card.state = core::Card::State::New;
        new_card.incorrect_streak = 0;
        new_card.interval = 0;
        new_card.next_review = now;
        new_card.created_at = now;
        new_card.updated_at = now;
        new_card.front = card.front;
        new_card.back = card.back;

        auto create_res = m_deps.deck_storage.create_cards({new_card});

        if (!create_res) {
            auto error = from_storage_error(create_res.error(), "DeckService::create_card(): failed to create card");

            dispatch(card_create_failed{error});
            return std::unexpected(error);
        }

        if (create_res->empty()) {
            auto error =
                Error::internal_error("DeckService::create_card(): after creating, storage did not return the created card");

            dispatch(card_create_failed{error});
            return std::unexpected(error);
        }

        dispatch(card_created{create_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<core::Card>> DeckService::card(core::Card::id_type id) const {
    return QtConcurrent::run([this, id]() -> core::IDeckService::Result<core::Card> {
        auto fetch_res = m_deps.deck_storage.fetch_cards(QVector<core::Card::id_type>{id});

        if (!fetch_res) {
            auto error = from_storage_error(fetch_res.error(), "DeckService::card(): failed to fetch card");

            dispatch(card_fetch_failed{error});
            return std::unexpected(error);
        }

        if (fetch_res->empty()) {
            auto error = Error::not_found("DeckService::card(): card not found");

            dispatch(card_fetch_failed{error});
            return std::unexpected(error);
        }

        return fetch_res->first();
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::remove_card(core::Card::id_type id) {
    return QtConcurrent::run([this, id]() -> core::IDeckService::Result<void> {
        auto remove_res = m_deps.deck_storage.remove_cards({id});

        if (!remove_res) {
            auto error = from_storage_error(remove_res.error(), "DeckService::remove_card(): failed to remove card");

            dispatch(card_remove_failed{error});
            return std::unexpected(error);
        }

        if (remove_res->empty()) {
            auto error =
                Error::internal_error("DeckService::remove_card(): after removing, storage did not return the removed card");

            dispatch(card_remove_failed{error});
            return std::unexpected(error);
        }

        dispatch(card_removed{remove_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<void>> DeckService::update_card(core::Card::id_type id, const UpdateCardDraft& card) {
    return QtConcurrent::run([this, id, card]() -> core::IDeckService::Result<void> {
        if (card.front.isEmpty()) {
            auto error = Error::validation_failed("DeckService::update_card(): card front is empty");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        if (card.back.isEmpty()) {
            auto error = Error::validation_failed("DeckService::update_card(): card back is empty");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        if (card.difficulty < 0.0f || card.difficulty > 5.0f) {
            auto error = Error::validation_failed("DeckService::update_card(): card difficulty is out of range");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        auto fetch_res = m_deps.deck_storage.fetch_cards(QVector<core::Card::id_type>({id}));

        if (!fetch_res) {
            auto error = from_storage_error(fetch_res.error(), "DeckService::update_card(): failed to fetch card");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        if (fetch_res->empty()) {
            auto error = Error::not_found("DeckService::update_card(): card not found");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        core::Card fetched_card = fetch_res->first();

        fetched_card.front = card.front;
        fetched_card.back = card.back;
        fetched_card.difficulty = card.difficulty;
        fetched_card.updated_at = QDateTime::currentDateTime();

        auto update_res = m_deps.deck_storage.update_cards({fetched_card});

        if (!update_res) {
            auto error = from_storage_error(update_res.error(), "DeckService::update_card(): failed to update card");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        if (update_res->empty()) {
            auto error =
                Error::internal_error("DeckService::update_card(): after updating, card storage did not return the updated card");

            dispatch(card_update_failed{error});
            return std::unexpected(error);
        }

        dispatch(card_updated{update_res->first()});
        return {};
    });
}


QFuture<core::IDeckService::Result<QVector<core::DeckSummary>>> DeckService::deck_summaries() const {
    return QtConcurrent::run([this]() -> core::IDeckService::Result<QVector<core::DeckSummary>> {
        auto decks_res = m_deps.deck_storage.fetch_decks();

        if (!decks_res) {
            auto error = from_storage_error(decks_res.error(), "DeckService::deck_summaries(): failed to fetch decks");
            return std::unexpected(error);
        }

        QVector<core::DeckSummary> summaries;
        summaries.reserve(decks_res->size());

        for (const auto& deck : *decks_res) {
            auto study_info = m_deps.study_engine.get_study_info(deck.id);

            if (!study_info) {
                auto error = Error::internal_error("DeckService::deck_summaries(): failed to get study info");
                return std::unexpected(error);
            }

            core::DeckSummary summary{.deck = deck,
                                      .new_cards = qMin(deck.new_limit, study_info->new_cards),
                                      .review_cards = qMin(deck.review_limit, study_info->consolidate_cards),
                                      .incorrect_cards = qMin(deck.incorrect_limit, study_info->incorrect_cards)};

            summaries.push_back(std::move(summary));
        }

        return summaries;
    });
}


QFuture<core::IDeckService::Result<QVector<core::Card>>> DeckService::cards(core::Deck::id_type deck_id) const {
    return QtConcurrent::run([this, deck_id]() -> core::IDeckService::Result<QVector<core::Card>> {
        auto fetch_res = m_deps.deck_storage.fetch_cards(deck_id);

        if (!fetch_res) {
            auto error = from_storage_error(fetch_res.error(), "DeckService::cards(): failed to fetch cards");

            dispatch(card_fetch_failed{error});
            return std::unexpected(error);
        }

        return *fetch_res;
    });
}


QFuture<core::IDeckService::Result<QVector<core::Card>>> DeckService::search_cards(const core::CardFilterChain& filters) const {
    return QtConcurrent::run([this, filters]() -> core::IDeckService::Result<QVector<core::Card>> {
        auto search_res = m_deps.search_engine.search_cards(filters);

        if (!search_res) {
            auto msg = QString("DeckService::search_cards(): failed to search cards: %1").arg(search_res.error().message);
            return std::unexpected(Error::internal_error(msg));
        }

        return *search_res;
    });
}


core::IDeckService::Error DeckService::from_storage_error(const core::IDeckStorage::Error& error, QString message) const {
    switch (error.kind) {
    case core::IDeckStorage::ErrorKind::AlreadyExists:
        return Error::already_exists(QString("%1: %2").arg(message).arg(error.message));

    case core::IDeckStorage::ErrorKind::NotFound:
        return Error::not_found(QString("%1: %2").arg(message).arg(error.message));

    default:
        return Error::internal_error(QString("%1: %2").arg(message).arg(error.message));
    }
}

} // namespace engine
