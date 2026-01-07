// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <DeckService/DeckService.hpp>     // for DeckService
#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter
#include <QtConcurrent/QtConcurrent>       // for QtConcurrent
#include <Utils/ScopeGuard.hpp>            // for ScopeGuard
#include <algorithm>                       // for std::find_if

namespace revise {

// Public method
DeckService::DeckService(std::function<std::unique_ptr<IDeckRepository>()> repo_factory,
                         IDeckImporter&                                    anki_importer,
                         IDeckImporter&                                    revise_importer,
                         IDeckExporter&                                    exporter,
                         DeckMediaStorage&                                 media_storage,
                         HtmlHelper&                                       html_helper,
                         QObject*                                          parent) :
    QObject(parent), m_repo_factory(std::move(repo_factory)), m_anki_importer(anki_importer),
    m_revise_importer(revise_importer), m_exporter(exporter), m_media_storage(media_storage),
    m_html_helper(html_helper) {
    m_repo = m_repo_factory();
}

// Public method
std::expected<QVector<DeckSummary>, QString> DeckService::list_decks(const QDateTime& now) {
    return m_repo->get_deck_summaries(now);
}

// Public method
std::expected<QVector<Card>, QString> DeckService::list_cards(int deckId) {
    return m_repo->get_cards(deckId);
}

// Public method
void DeckService::create_deck(const QString& name,
                              const QString& description,
                              int            time_limit,
                              int            new_limit,
                              int            consolidate_limit,
                              int            incorrect_limit) {

    auto res = m_repo->create_deck(Deck{.name = name.trimmed(),
                                        .description = description.trimmed(),
                                        .time_limit = time_limit,
                                        .new_limit = new_limit,
                                        .consolidate_limit = consolidate_limit,
                                        .incorrect_limit = incorrect_limit});

    if (!res.has_value()) {
        emit errorOccured(res.error());
        return;
    }

    emit deckCreated();
    emit decksUpdated();
}

// Public method
void DeckService::remove_deck(int deckId) {
    auto res = m_repo->delete_deck(deckId);

    if (!res.has_value()) {
        ErrorReporter::instance()->report("Failed to remove deck", res.error(), "DeckService::remove_deck");
        emit decksUpdated(); // maybe updated
        return;
    }

    emit deckRemoved();
    emit decksUpdated();
}

// Public method
void DeckService::import_deck_async(const QString& path) {
    if (m_import_in_progress) {
        return;
    }

    [[maybe_unused]] QFuture res = QtConcurrent::run([this, path]() {
        ScopeGuard guard(
            [this]() { // on enter
                m_import_in_progress = true;
                emit importInProgressChanged();
            },
            [this]() { // on exit
                m_import_in_progress = false;
                emit importInProgressChanged();
            });

        auto extract_extension = [](const QString& filename) -> QString {
            // Matches ".apkg", ".zip", ".anki" etc. before optional " (n)"
            static const QRegularExpression re(R"(\.([A-Za-z0-9]+)(?:\s*\(\d+\))?$)");

            auto match = re.match(filename);
            if (!match.hasMatch()) {
                return {};
            }

            return match.captured(1).toLower();
        };

        std::expected<ImportResult, QString> import_res;

        // Select importer automatically
        if (auto suffix = extract_extension(QFileInfo(path).fileName()); suffix == "apkg") {
            import_res = m_anki_importer.import_from_file(path);
        } else if (suffix == "rpkg") {
            import_res = m_revise_importer.import_from_file(path);
        } else {
            ErrorReporter::instance()->report(
                "Unknown file extension: " + suffix, "Can't select importer", "DeckService::import_deck_async()");
            return;
        }

        QVector<CardBackUpdate> cards_to_update;
        QVector<Card>           cards_to_insert;

        if (!import_res.has_value()) {
            QMetaObject::invokeMethod(this, [this, import_res]() {
                ErrorReporter::instance()->report(
                    "Failed to import deck", import_res.error(), "DeckService::import_deck");
            });
            return;
        }

        // We need a separate connection to the database.
        // So we need to create separate IDeckRepository
        auto repo = m_repo_factory();

        if (repo == nullptr) {
            ErrorReporter::instance()->report("Failed to create separate IDeckRepository",
                                              "m_repo_factory() returned nullptr",
                                              "DeckService::import_deck_async()");
            return;
        }

        // Check if a deck with that name exists
        bool deck_exists{false};
        if (auto res = repo->is_deck_exists(import_res->deck_name); !res.has_value()) {
            ErrorReporter::instance()->report(
                QString("Could not verify the existence of a deck with the name: %1").arg(import_res->deck_name),
                res.error(),
                "DeckService::import_deck_async()");
            return;
        } else {
            deck_exists = res.value();
        }

        int deckId{0};

        // Get ID if deck exists
        if (deck_exists) {
            if (auto res = repo->deck_id(import_res->deck_name); !res.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Unable to get the ID of the deck named: %1").arg(import_res->deck_name),
                    res.error(),
                    "DeckService::import_deck_async()");
                return;
            } else {
                deckId = res.value();
            }
        } else {
            // else create new deck
            if (auto res = repo->create_deck(Deck{.name = import_res->deck_name,
                                                  .description = import_res->deck_description,
                                                  .time_limit = 0,
                                                  .new_limit = 30,
                                                  .consolidate_limit = 30,
                                                  .incorrect_limit = 30});
                !res.has_value()) {

                ErrorReporter::instance()->report(
                    "Failed to create deck", res.error(), "DeckService::import_deck_async()");
                return;
            } else {
                deckId = res.value();
            }
        }

        // Update deck parameters
        if (auto res = repo->update_deck(Deck{.name = import_res->deck_name,
                                              .description = import_res->deck_description,
                                              .time_limit = import_res->time_limit,
                                              .new_limit = import_res->new_limit,
                                              .consolidate_limit = import_res->consolidate_limit,
                                              .incorrect_limit = import_res->incorrect_limit,
                                              .id = deckId});
            !res.has_value()) {
            ErrorReporter::instance()->report("Failed to update deck", res.error(), "DeckService::import_deck_async()");
            return;
        }

        // Save all images
        QMap<QString, QByteArray> images = import_res->user_data.value<QMap<QString, QByteArray>>();
        QMap<QString /* image name */, QString /* path to image */> mapped_images;
        for (const auto& [name, data] : images.toStdMap()) {
            if (auto res = m_media_storage.save_image(deckId, name, data); !res.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Failed to save image with name %1, deck ID = %2").arg(name).arg(deckId),
                    res.error(),
                    "DeckService::import_deck_async()");
                return;
            } else {
                mapped_images[name] = "file://" + res.value();
            }
        }

        // Update img sources
        for (auto& imported_card : import_res->cards) {
            imported_card.back = m_html_helper.replace_image_src(imported_card.back, mapped_images);
        }

        // Update cards if deck exists
        if (deck_exists) {
            auto cards = repo->get_cards(deckId);

            // error
            if (!cards.has_value()) {
                ErrorReporter::instance()->report(QString("Failed to fetch cards from deck with ID = %1").arg(deckId),
                                                  cards.error(),
                                                  "DeckService::import_deck_async()");
                return;
            }

            // update cards
            for (const auto& card : import_res->cards) {
                auto iter = std::find_if(cards.value().begin(), cards.value().end(), [&card](const Card& i) {
                    return card.front == i.front;
                });

                // if card found in database
                if (iter != cards.value().end()) {
                    // update card and save to vector
                    cards_to_update.push_back(CardBackUpdate{
                        .cardId = iter->id, .back = card.back, .updated_at = QDateTime::currentDateTime()});
                } else {
                    // insert card to db
                    cards_to_insert.push_back(std::move(card));
                }
            }
        } else {
            // Insert cards if deck not exists
            cards_to_insert = std::move(import_res->cards);
        }

        // Update old
        if (auto res = repo->update_card_backs_batch(cards_to_update); !res.has_value()) {
            ErrorReporter::instance()->report(
                "Failed to update card backs batch", res.error(), "DeckService::import_deck_async()");
            return;
        }

        // Insert new
        if (auto res = repo->insert_cards_batch(deckId, cards_to_insert); !res.has_value()) {
            ErrorReporter::instance()->report(
                "Failed to insert cards batch", res.error(), "DeckService::import_deck_async()");
            return;
        }

        // Finish
        QMetaObject::invokeMethod(this, &DeckService::decksUpdated, Qt::QueuedConnection);
        QMetaObject::invokeMethod(
            this,
            [this, card_count = cards_to_insert.size()]() { emit deckImported(card_count); },
            Qt::QueuedConnection);
    });
}

// Public method
void DeckService::export_deck_async(int deckId, const QString& path) {
    if (m_export_in_progress) {
        return;
    }

    [[maybe_unused]] QFuture res = QtConcurrent::run([this, path, deckId]() {
        ScopeGuard guard(
            [this]() { // on enter
                m_export_in_progress = true;
                emit exportInProgressChanged();
            },
            [this]() { // on exit
                m_export_in_progress = false;
                emit exportInProgressChanged();
            });

        // Create local repo
        auto repo = m_repo_factory();

        if (repo == nullptr) {
            ErrorReporter::instance()->report("Failed to create local deck repository",
                                              "m_repo_factory() == nullptr",
                                              "DeckService::export_deck_async()");
            return;
        }

        // Variables
        Deck          deck;
        QVector<Card> cards;
        const QString media_dir = m_media_storage.get_deck_media_folder(deckId);

        // Fetch deck
        if (auto res = repo->get_deck(deckId); res.has_value()) {
            deck = std::move(*res);
        } else {
            ErrorReporter::instance()->report(
                "Failed to fetch deck from repository", res.error(), "DeckService::export_deck_async()");
            return;
        }

        // Fetch card
        if (auto res = repo->get_cards(deckId); res.has_value()) {
            cards = std::move(*res);
        } else {
            ErrorReporter::instance()->report(
                "Failed to fetch cards from repository", res.error(), "DeckService::export_deck_async()");
            return;
        }

        // Build summary and request export
        auto export_res = m_exporter.export_to_file(
            ExportData{.deck = std::move(deck), .media_directory = std::move(media_dir), .cards = std::move(cards)},
            path);

        if (!export_res.has_value()) {
            ErrorReporter::instance()->report(
                "Failed to export", export_res.error(), "DeckService::export_deck_async()");
            return;
        }

        QMetaObject::invokeMethod(this, &DeckService::deckExported, Qt::QueuedConnection);
    });
}

// Public method
Deck DeckService::deck_info(int deckId) {
    auto res = m_repo->get_deck(deckId);

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to get deck info with id: %1").arg(deckId), res.error(), "DeckService::deck_info()");
        return Deck{};
    }

    return res.value();
}

// Public method
void DeckService::update_deck(int            deckId,
                              const QString& name,
                              const QString& description,
                              int            time_limit,
                              int            new_limit,
                              int            consolidate_limit,
                              int            incorrect_limit) {
    auto res = m_repo->update_deck(Deck{.name = name,
                                        .description = description,
                                        .time_limit = time_limit,
                                        .new_limit = new_limit,
                                        .consolidate_limit = consolidate_limit,
                                        .incorrect_limit = incorrect_limit,
                                        .id = deckId});

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to update deck with id: %1").arg(deckId), res.error(), "DeckService::update_deck");
        return;
    }

    emit deckUpdated();
}

// Public method
bool DeckService::is_deck_exists(const QString& deck_name) {
    auto res = m_repo->is_deck_exists(deck_name.trimmed());

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Unable to verify whether a deck with the name exists: %1").arg(deck_name),
            res.error(),
            "DeckService::is_deck_exists()");
        return false;
    }

    return res.value();
}

// Public method
bool DeckService::is_card_exists(int deckId, const QString& card_front) {
    auto res = m_repo->is_card_exists(deckId, card_front.trimmed());

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Unable to verify whether a card with the front '%1' and deck id '%2' exists")
                .arg(card_front)
                .arg(deckId),
            res.error(),
            "DeckService::is_card_exists()");
        return false;
    }

    return res.value();
}

// Public method
void DeckService::create_card(int deckId, const QString& front, const QString& back) {
    auto res = m_repo->insert_card(deckId,
                                   Card{.state = 0,
                                        .incorrect_streak = 0,
                                        .interval = 0,
                                        .difficulty = 2.5,
                                        .next_review = QDateTime::currentDateTime(),
                                        .created_at = QDateTime::currentDateTime(),
                                        .updated_at = QDateTime::currentDateTime(),
                                        .front = front,
                                        .back = back});

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Unable to insert card with front = %1, back = %2, deckId = %3").arg(front).arg(back).arg(deckId),
            res.error(),
            "DeckService::create_card()");
        return;
    }

    emit deckUpdated();
}

// Public method
Card DeckService::get_card(int cardId) {
    auto res = m_repo->get_card(cardId);

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Unable to fetch card with id = %1").arg(cardId), res.error(), "DeckService::get_card()");
        return Card{};
    }

    return res.value();
}

// Public method
void DeckService::remove_card(int cardId) {
    auto res = m_repo->remove_card(cardId);

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to remove card with id = %1").arg(cardId), res.error(), "DeckService::remove_card()");
        return;
    }

    emit deckUpdated();
    emit cardRemoved();
}

// Public method
void DeckService::update_card(int cardId, const QString& front, const QString& back) {
    auto fetching_res = m_repo->get_card(cardId);

    if (!fetching_res.has_value()) {
        ErrorReporter::instance()->report(QString("Failed to fetch info about card with id = %1").arg(cardId),
                                          fetching_res.error(),
                                          "DeckService::update_card()");
        return;
    }

    auto res = m_repo->update_card({.id = cardId,
                                    .deck_id = fetching_res->deck_id,
                                    .state = fetching_res->state,
                                    .incorrect_streak = fetching_res->incorrect_streak,
                                    .interval = fetching_res->interval,
                                    .time_limit = fetching_res->time_limit,
                                    .difficulty = fetching_res->difficulty,
                                    .next_review = fetching_res->next_review,
                                    .created_at = fetching_res->created_at,
                                    .updated_at = QDateTime::currentDateTime(),
                                    .front = front,
                                    .back = back});

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to update card with id = %1").arg(cardId), res.error(), "DeckService::update_card()");
        return;
    }

    emit deckUpdated();
    emit cardUpdated();
}

// Public method
bool DeckService::import_in_progress() const noexcept {
    return m_import_in_progress;
}

// Public method
bool DeckService::export_in_progress() const noexcept {
    return m_export_in_progress;
}

} // namespace revise
