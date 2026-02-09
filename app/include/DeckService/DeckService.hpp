// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <DeckMediaStorage/DeckMediaStorage.hpp> // for DeckMediaStorage
#include <HtmlHelper/HtmlHelper.hpp>             // for HtmlHelper
#include <IDeckExporter/IDeckExporter.hpp>       // for IDeckExporter
#include <IDeckImporter/IDeckImporter.hpp>       // for IDeckImporter
#include <IDeckRepository/IDeckRepository.hpp>   // for IDeckRepository
#include <QObject>                               // for QObject
#include <functional>                            // for std::function
#include <memory>                                // for std::unique_ptr

namespace revise {

/**
 * @brief Application service for decks. This class can be used to manage decks, including exporting/importing them.
 */
class DeckService : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(bool importInProgress READ import_in_progress NOTIFY importInProgressChanged)
    Q_PROPERTY(bool exportInProgress READ export_in_progress NOTIFY exportInProgressChanged)

    DeckService(std::function<std::unique_ptr<IDeckRepository>()> repo_factory,
                IDeckImporter&                                    anki_importer,
                IDeckImporter&                                    revise_importer,
                IDeckExporter&                                    exporter,
                DeckMediaStorage&                                 media_storage,
                HtmlHelper&                                       html_helper,
                QObject*                                          parent = nullptr);

    /**
     * @brief Get a list of decks
     * @details For each deck, statistics will be calculated **at the current time `now`** - how many new cards, how
     * many to repeat, how many incorrect ones.
     * @param now The time for which statistics are compiled
     * @return `std::expected<QVector<DeckSummary>, QString>`
     */
    std::expected<QVector<DeckSummary>, QString> list_decks(const QDateTime& now = QDateTime::currentDateTime());

    /**
     * @brief Get all cards belonging to a specific deck
     * @param deckId Deck ID
     * @return `std::expected<QVector<Card>, QString>`
     */
    std::expected<QVector<Card>, QString> list_cards(int deckId);

    /**
     * @brief create_deck Create deck
     * @param name Name of the deck
     * @param description Description of the deck
     * @param time_limit Time limit of the deck
     * @param new_limit Limit on the number of new cards per day
     * @param consolidate_limit Limit on the number of consolidate cards per day
     * @param incorrect_limit Limit on the number of incorrect cards per day
     * @return `std::expected<int, QString>`
     * @note Observe signal `deckCreated`
     */
    Q_INVOKABLE void create_deck(const QString& name,
                                 const QString& description = QString(),
                                 int            time_limit = 0,
                                 int            new_limit = 30,
                                 int            consolidate_limit = 30,
                                 int            incorrect_limit = 30);

    /**
     * @brief Remove deck
     * @param deckId ID of the deck
     * @note Observe signal `deckRemoved`
     */
    Q_INVOKABLE void remove_deck(int deckId);

    /**
     * @brief Import `anki` or `revise` deck async. The selection of the desired importer occurs **automatically**.
     * @param path Path to the deck
     * @note If successful, the `deckImported` signal will be emitted. If an error occurs,
     * it will be logged in `ErrorReporter`.
     * @note If a deck with that name already exists, it will be overwritten, but the
     * current card statistics (training) will remain.
     */
    Q_INVOKABLE void import_deck_async(const QString& path);

    /**
     * @brief Exports the deck in `Revise` format (`rpkg`)
     * @param deckId ID of the deck to be exported
     * @param path Where to save the result
     */
    Q_INVOKABLE void export_deck_async(int deckId, const QString& path);

    /**
     * @brief Get deck info by deck id
     * @param deckId ID of deck
     * @return `Deck`
     */
    Q_INVOKABLE Deck deck_info(int deckId);

    /**
     * @brief Update deck
     * @param deckId ID of the deck
     * @param name Name of the deck
     * @param description Description of the deck
     * @param time_limit Time limit of the deck
     * @param new_limit Limit on the number of new cards per day
     * @param consolidate_limit Limit on the number of consolidate cards per day
     * @param incorrect_limit Limit on the number of incorrect cards per day
     * @return `std::expected<int, QString>`
     * @note Observe signal `deckUpdated`
     */
    Q_INVOKABLE void update_deck(int            deckId,
                                 const QString& name,
                                 const QString& description,
                                 int            time_limit,
                                 int            new_limit,
                                 int            consolidate_limit,
                                 int            incorrect_limit);

    /**
     * @brief Checks whether a deck with that name exists in the database
     * @param deck_name Name of the deck
     * @return `true` if exists
     */
    Q_INVOKABLE bool is_deck_exists(const QString& deck_name);

    /**
     * @brief Checks whether a card exists in the specified deck
     * @param deckId ID of the deck
     * @param card_front Front of the card
     * @return `true` if exists
     */
    Q_INVOKABLE bool is_card_exists(int deckId, const QString& card_front);

    /**
     * @brief Create a card
     * @param deckId ID of the deck to which the card will belong
     * @param front Front of the card
     * @param back Back of the card
     */
    Q_INVOKABLE void create_card(int deckId, const QString& front, const QString& back);

    /**
     * @brief Returns information about the card
     * @param cardId Card ID
     * @return `Card`
     */
    Q_INVOKABLE Card get_card(int cardId);

    /**
     * @brief Remove card
     * @param cardId ID of the card
     */
    Q_INVOKABLE void remove_card(int cardId);

    /**
     * @brief Update card
     * @param cardId ID of the card
     * @param front Front of the card
     * @param back Back of the card
     */
    Q_INVOKABLE void update_card(int cardId, const QString& front, const QString& back);

    /**
     * @brief Is the deck being imported now?
     * @return `true` if true
     */
    bool import_in_progress() const noexcept;

    /**
     * @brief Is the deck being exported now?
     * @return `true` if true
     */
    bool export_in_progress() const noexcept;

  signals:
    void deckCreated();
    void deckRemoved();
    void deckImported(int cardsImported);
    void deckExported();
    void decksUpdated();
    void deckUpdated();
    void cardRemoved();
    void cardUpdated();
    void importInProgressChanged();
    void exportInProgressChanged();
    void errorOccured(const QString& what);

  private:
    std::function<std::unique_ptr<IDeckRepository>()> m_repo_factory;
    IDeckImporter&                                    m_anki_importer;
    IDeckImporter&                                    m_revise_importer;
    IDeckExporter&                                    m_exporter;
    DeckMediaStorage&                                 m_media_storage;
    HtmlHelper&                                       m_html_helper;
    std::unique_ptr<IDeckRepository>                  m_repo; ///< Create in ctor

    bool m_import_in_progress{false};
    bool m_export_in_progress{false};
};

} // namespace revise
