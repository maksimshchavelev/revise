// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <IDeckRepository/IDeckRepository.hpp> // for IDeckRepository
#include <QObject>                             // for QObject

namespace revise {

/**
 * @brief Application service for decks. This class can be used to manage decks, including exporting/importing them.
 */
class DeckService : public QObject {
    Q_OBJECT

  public:
    DeckService(IDeckRepository& repository, QObject* parent = nullptr);

    /**
     * @brief Get a list of decks
     * @details For each deck, statistics will be calculated **at the current time `now`** - how many new cards, how
     * many to repeat, how many incorrect ones.
     * @param now The time for which statistics are compiled
     * @return `std::expected<QVector<DeckSummary>, QString>`
     */
    std::expected<QVector<DeckSummary>, QString> list_decks(const QDateTime& now = QDateTime::currentDateTime());

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
    Q_INVOKABLE void delete_deck(int deckId);

  signals:
    void deckCreated();
    void deckRemoved();
    void decksUpdated();
    void errorOccured(const QString& what);

  private:
    IDeckRepository& m_repository;
};

} // namespace revise
