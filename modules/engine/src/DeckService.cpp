// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/DeckService.hpp"    // for header
#include <QFuture>                   // for QFuture
#include <QtConcurrent/QtConcurrent> // for QtConcurrent
#include <QtGlobal>                  // for math
#include <QtMinMax>                  // for min / max
#include <utils/Html.hpp>            // for html utils
#include <utils/ScopeGuard.hpp>      // for ScopeGuard

namespace engine {

DeckService::DeckService(DeckServiceDeps deps) : m_deps(deps) {}


std::expected<void, QString> DeckService::create_deck(const core::Deck& deck) {
    auto res = m_deps.deck_storage.create_decks({deck});

    if (res) {
        dispatch(deck_created{});
        dispatch(decks_updated{});
    }

    return res;
}


std::expected<void, QString> DeckService::remove_deck(int deck_id) {
    auto res = m_deps.deck_storage.delete_decks({deck_id});

    if (res) {
        dispatch(deck_removed{});
        dispatch(decks_updated{});
    }

    m_deps.deck_media_storage.remove_deck_media(deck_id);

    return res;
}


void DeckService::import_deck_async(const QString& path) {
    if (m_import_in_progress) {
        return;
    }

    [[maybe_unused]] QFuture res = QtConcurrent::run([this, path]() {
        utils::ScopeGuard guard(
            [this]() { // on enter
                m_import_in_progress = true;
                dispatch(import_started{});
            },
            [this]() { // on exit
                m_import_in_progress = false;
                dispatch(import_finished{});
            });

        const QUrl url(path);

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
            dispatch(import_failed{QString("Unknown file extension: %1 (cannot select importer for file %2)")
                                       .arg(suffix)
                                       .arg(local_path)});
            return;
        }

        QVector<core::Card> cards_to_update;
        QVector<core::Card> cards_to_insert;

        if (!import_res) {
            dispatch(import_failed{QString("Failed to import deck, cause: %1").arg(import_res.error())});
        }

        // Check if a deck with that name exists
        auto deck_fetch_request = m_deps.deck_storage.fetch_decks({import_res->deck_name});

        if (!deck_fetch_request) {
            dispatch(import_failed{
                QString("Could not verify the existence of a deck with the name: %1").arg(import_res->deck_name)});
        }

        bool deck_exists = deck_fetch_request->size() > 0;
        int  deck_id{0};

        // Get ID if deck exists
        if (deck_exists) {
            deck_id = deck_fetch_request->first().id;
        } else {
            // else create new deck
            if (auto res = m_deps.deck_storage.create_decks({core::Deck{.name = import_res->deck_name,
                                                                        .description = import_res->deck_description,
                                                                        .time_limit = 0,
                                                                        .new_limit = 30,
                                                                        .review_limit = 30,
                                                                        .incorrect_limit = 30}});
                !res) {
                dispatch(import_failed{QString("Could not create deck with name: %1").arg(import_res->deck_name)});
                return;
            }

            deck_fetch_request = m_deps.deck_storage.fetch_decks({import_res->deck_name});

            if (!deck_fetch_request) {
                dispatch(
                    import_failed{QString("Could not fetch created deck with name: %1").arg(import_res->deck_name)});
                return;
            }

            deck_id = deck_fetch_request->first().id;
        }

        // Update deck parameters
        if (auto res = m_deps.deck_storage.update_decks({core::Deck{.name = import_res->deck_name,
                                                                    .description = import_res->deck_description,
                                                                    .time_limit = import_res->time_limit,
                                                                    .new_limit = import_res->new_limit,
                                                                    .review_limit = import_res->consolidate_limit,
                                                                    .incorrect_limit = import_res->incorrect_limit,
                                                                    .id = deck_id}});
            !res) {
            dispatch(import_failed{QString("Failed to update deck, cause: %1").arg(res.error())});
        }

        // Save all images
        QMap<QString, QByteArray> images = import_res->user_data.value<QMap<QString, QByteArray>>();
        QMap<QString /* image name */, QString /* path to image */> mapped_images;
        for (const auto& [name, data] : images.toStdMap()) {
            if (auto res = m_deps.deck_media_storage.save_image(deck_id, name, data); !res) {
                dispatch(import_failed{QString("Failed to save deck image with name %1 (deck id: %2), cause: %3")
                                           .arg(name)
                                           .arg(deck_id)
                                           .arg(res.error())});
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
                dispatch(import_failed{QString("Failed to fetch cards from deck with ID = %1, cause: %2")
                                           .arg(deck_id)
                                           .arg(cards.error())});
                return;
            }

            // update cards
            for (const auto& card : import_res->cards) {
                auto iter = std::find_if(cards.value().begin(), cards.value().end(), [&card](const core::Card& i) {
                    return card.front == i.front;
                });

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
            dispatch(import_failed{QString("Failed to update cards batch")});
            return;
        }

        // Insert new
        if (auto res = m_deps.deck_storage.insert_cards(cards_to_insert); !res) {
            dispatch(import_failed{QString("Failed to insert cards batch")});
            return;
        }

        dispatch(decks_updated{});
        dispatch(deck_imported{});
    });
}


void DeckService::export_deck_async(int deck_id, const QString& path) {
    if (m_export_in_progress) {
        return;
    }

    [[maybe_unused]] QFuture res = QtConcurrent::run([this, path, deck_id]() {
        utils::ScopeGuard guard(
            [this]() { // on enter
                m_export_in_progress = true;
                dispatch(export_started{});
            },
            [this]() { // on exit
                m_export_in_progress = false;
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
            dispatch(export_failed{
                QString("Failed to fetch deck with id = %1 from storage, cause: %2").arg(deck_id).arg(res.error())});
            return;
        }

        // Fetch card
        if (auto res = m_deps.deck_storage.fetch_cards(deck_id); res) {
            cards = std::move(*res);
        } else {
            dispatch(export_failed{QString("Failed to fetch cards for deck with id = %1 from storage, cause: %2")
                                       .arg(deck_id)
                                       .arg(res.error())});
            return;
        }

        auto export_res = m_deps.deck_exporter.export_to_file(
            core::ExportData{
                .deck = std::move(deck), .media_directory = std::move(media_dir), .cards = std::move(cards)},
            local_path);

        if (!export_res) {
            dispatch(export_failed{
                QString("Failed to export deck with id = %1, cause: %2").arg(deck_id).arg(export_res.error())});
            return;
        }

        dispatch(deck_exported{});
    });
}


std::expected<core::Deck, QString> DeckService::deck(int deck_id) {
    auto res = m_deps.deck_storage.fetch_decks(QVector<int>{deck_id});

    if (!res) {
        return std::unexpected(res.error());
    }

    if (res->empty()) {
        return std::unexpected(QString("DeckService::deck() fetched empty deck with id %1").arg(deck_id));
    }

    return res->first();
}


std::expected<void, QString> DeckService::update_deck(const core::Deck& deck) {
    auto res = m_deps.deck_storage.update_decks({deck});

    if (res) {
        dispatch(deck_updated{});
        dispatch(decks_updated{});
    }

    return res;
}


std::expected<void, QString> DeckService::create_card(const core::Card& card) {
    auto res = m_deps.deck_storage.insert_cards({normalize_card(card)});

    if (res) {
        dispatch(card_created{});
    }

    return res;
}


std::expected<core::Card, QString> DeckService::card(int id) {
    auto res = m_deps.deck_storage.fetch_cards(QVector<int>{id});

    if (!res) {
        return std::unexpected(res.error());
    }

    return res->first();
}


std::expected<void, QString> DeckService::remove_card(int id) {
    auto res = m_deps.deck_storage.remove_cards({id});

    if (res) {
        dispatch(card_removed{});
    }

    return res;
}


std::expected<void, QString> DeckService::update_card(const core::Card& card) {
    auto res = m_deps.deck_storage.update_cards({normalize_card(card)});

    if (res) {
        dispatch(cards_updated{});
    }

    return res;
}


std::expected<QVector<core::DeckSummary>, QString> DeckService::deck_summaries() {
    auto decks = m_deps.deck_storage.fetch_decks();

    if (!decks) {
        auto err = QString("DeckService::deck_summaries(): failed to fetch decks: '%1'").arg(decks.error());
        return std::unexpected(std::move(err));
    }

    QVector<core::DeckSummary> summaries;
    summaries.reserve(decks->size());

    for (const auto& deck : decks.value()) {
        auto study_info = m_deps.study_engine.get_study_info(deck.id);

        if (!study_info) {
            auto err = QString("DeckService::deck_summaries(): failed to get study info: '%1'").arg(study_info.error());
            return std::unexpected(std::move(err));
        }

        core::DeckSummary s{.deck = deck,
                            .new_cards = qMin(deck.new_limit, study_info->new_cards),
                            .consolidate_cards = qMin(deck.review_limit, study_info->consolidate_cards),
                            .incorrect_cards = qMin(deck.incorrect_limit, study_info->incorrect_cards)};

        summaries.push_back(std::move(s));
    }

    return summaries;
}


std::expected<QVector<core::Card>, QString> DeckService::cards(int deck_id) {
    return m_deps.deck_storage.fetch_cards(deck_id);
}


std::expected<QVector<core::Card>, core::SearchError> DeckService::search_cards(const core::CardFilterChain& filters) {
    return m_deps.search_engine.search_cards(filters);
}


core::Card DeckService::normalize_card(const core::Card& card) {
    core::Card copy = card;

    copy.difficulty = qBound(0.0f, copy.difficulty, 5.0f);

    if (copy.global_id < 0) {
        copy.global_id = 0;
    }

    if (copy.id < 0) {
        copy.id = 0;
    }

    if (copy.interval < 0) {
        copy.interval = 0;
    }

    return copy;
}

} // namespace engine
