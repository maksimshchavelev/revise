// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <DeckService/DeckService.hpp>     // for DeckService
#include <ErrorReporter/ErrorReporter.hpp> // for ErrorReporter
#include <QtConcurrent/QtConcurrent>       // for QtConcurrent
#include <algorithm>                       // for std::find_if

namespace revise {

// Public method
DeckService::DeckService(std::function<std::unique_ptr<IDeckRepository>()> repo_factory,
                         IDeckImporter& importer,
                         QObject* parent) :
    QObject(parent),
    m_repo_factory(std::move(repo_factory)),
    m_importer(importer) {
    m_repo = m_repo_factory();
}

// Public method
std::expected<QVector<DeckSummary>, QString> DeckService::list_decks(const QDateTime& now) {
    return m_repo->get_deck_summaries(now);
}

// Public method
std::expected<QVector<Card>, QString> DeckService::list_cards(int deckId)
{
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
void DeckService::remove_deck(int deckId)
{
    auto res = m_repo->delete_deck(deckId);

    if (!res.has_value()) {
        emit errorOccured(res.error());
        return;
    }

    emit deckRemoved();
    emit decksUpdated();
}

// Public method
void DeckService::import_deck_async(const QString& path)
{
    if (m_import_in_progress) {
        return;
    }

    m_import_in_progress = true;
    emit importInProgressChanged();

    [[maybe_unused]] QFuture res = QtConcurrent::run([this, path](){
        auto import_res = m_importer.import_from_file(path);
        int  imported{0};

        if (!import_res.has_value()) {
            QMetaObject::invokeMethod(this, [this, import_res]() {
                ErrorReporter::instance()->report(
                    "Failed to import deck",
                    import_res.error(),
                    "DeckService::import_deck");
            });
            m_import_in_progress = false;
            emit importInProgressChanged();
            return;
        }

        // We need a separate connection to the database.
        // So we need to create separate IDeckRepository
        auto repo = m_repo_factory();

        if (repo == nullptr) {
            ErrorReporter::instance()->report(
                "Failed to create separate IDeckRepository",
                "m_repo_factory() returned nullptr",
                "DeckService::import_deck_async()"
            );
            m_import_in_progress = false;
            emit importInProgressChanged();
            return;
        }

        // Check if a deck with that name exists
        bool deck_exists{false};
        if (auto res = repo->is_deck_exists(import_res->deck_name); !res.has_value()) {
            ErrorReporter::instance()->report(
                QString("Could not verify the existence of a deck with the name: %1")
                    .arg(import_res->deck_name),
                res.error(),
                "DeckService::import_deck_async()"
            );
            m_import_in_progress = false;
            emit importInProgressChanged();
            return;
        } else {
            deck_exists = res.value();
        }

        int deckId{0};

        // Get ID if deck exists
        if (deck_exists) {
            if (auto res = repo->deck_id(import_res->deck_name); !res.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Unable to get the ID of the deck named: %1")
                        .arg(import_res->deck_name),
                    res.error(),
                    "DeckService::import_deck_async()"
                    );
                m_import_in_progress = false;
                emit importInProgressChanged();
                return;
            } else {
                deckId = res.value();
            }
        } else {
            // else create new deck
            if (auto res = repo->create_deck(Deck {
                    .name = import_res->deck_name,
                    .description = import_res->deck_description,
                    .time_limit = 0,
                    .new_limit = 30,
                    .consolidate_limit = 30,
                    .incorrect_limit = 30
                }); !res.has_value()) {

                ErrorReporter::instance()->report(
                    "Failed to create deck",
                    res.error(),
                    "DeckService::import_deck_async()"
                    );
                m_import_in_progress = false;
                emit importInProgressChanged();
                return;
            } else {
                deckId = res.value();
            }
        }

        // Update cards if deck exists
        // TODO TODO TODO TODO TODO TODO TODO TODO: implement updating deck, for example, time limit
        if (deck_exists) {
            auto cards = repo->get_cards(deckId);

            // error
            if (!cards.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Failed to fetch cards from deck with ID = %1").arg(deckId),
                    cards.error(),
                    "DeckService::import_deck_async()"
                );
                m_import_in_progress = false;
                emit importInProgressChanged();
                return;
            }

            // update cards
            QVector<Card> new_cards; // New cards that were not previously in the database

            for(const auto& card : import_res->cards) {
                auto iter = std::find_if(cards.value().begin(), cards.value().end(), [&card](const Card& i){
                    return card.front == i.front;
                });

                // if card found in database
                if (iter != cards.value().end()) {
                    // update card
                    iter->back = card.back;
                    iter->updated_at = QDateTime::currentDateTime();
                    iter->created_at = iter->created_at;
                } else {
                    // insert card to db
                    new_cards.push_back(std::move(card));
                }
            }

            // insert new cards
            if (auto res = repo->insert_cards_batch(deckId, new_cards); !res.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Failed to import card to deck with ID = %1").arg(deckId),
                    res.error(),
                    "DeckService::import_deck_async()"
                );
                m_import_in_progress = false;
                emit importInProgressChanged();
                return;
            }

            imported += new_cards.size();

            // update old cards
            for (const auto& card : import_res->cards) {
                if (auto res = repo->update_card(card); !res.has_value()) {
                    ErrorReporter::instance()->report(
                        QString("Failed to update card with front = %1").arg(card.front),
                        res.error(),
                        "DeckService::import_deck_async()"
                    );
                    m_import_in_progress = false;
                    emit importInProgressChanged();
                    return;
                }
            }

        } else {
            // Insert cards if deck not exists
            if (auto res = repo->insert_cards_batch(deckId, import_res->cards); !res.has_value()) {
                ErrorReporter::instance()->report(
                    QString("Failed to import card to deck with ID = %1").arg(deckId),
                    res.error(),
                    "DeckService::import_deck_async()"
                );
                m_import_in_progress = false;
                emit importInProgressChanged();
                return;
            }

            imported += import_res->cards.size();
        }

        // Finish
        m_import_in_progress = false;
        QMetaObject::invokeMethod(this, &DeckService::importInProgressChanged, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &DeckService::decksUpdated, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, [this, card_count = imported]() {
            emit deckImported(card_count);
        }, Qt::QueuedConnection);
    });
}

// Public method
Deck DeckService::deck_info(int deckId)
{
    auto res = m_repo->get_deck(deckId);

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to get deck info with id: %1").arg(deckId),
            res.error(),
            "DeckService::deck_info()");
        return Deck {};
    }

    return res.value();
}

// Public method
void DeckService::update_deck(int deckId,
                              const QString& name,
                              const QString& description,
                              int time_limit,
                              int new_limit,
                              int consolidate_limit,
                              int incorrect_limit)
{
    auto res = m_repo->update_deck(Deck {
        .name = name,
        .description = description,
        .time_limit = time_limit,
        .new_limit = new_limit,
        .consolidate_limit = consolidate_limit,
        .incorrect_limit = incorrect_limit,
        .id = deckId
    });

    if (!res.has_value()) {
        ErrorReporter::instance()->report(
            QString("Failed to update deck with id: %1").arg(deckId),
            res.error(),
            "DeckService::update_deck"
        );
        return;
    }

    emit deckUpdated();
}

// Public method
bool DeckService::import_in_progress() const noexcept
{
    return m_import_in_progress;
}

} // namespace revise
