// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <Database/Database.hpp>               // for Database
#include <IDeckRepository/IDeckRepository.hpp> // for IDeckRepository

namespace revise {

/**
 * @brief Repository interface for deck/card persistence.
 *
 * Implementations (e.g. SqlDeckRepository) provide concrete storage logic.
 */
class SqlDeckRepository : public QObject, public IDeckRepository {
    Q_OBJECT

  public:
    explicit SqlDeckRepository(Database& driver, QObject* parent = nullptr);
    virtual ~SqlDeckRepository() = default;

    std::expected<QVector<DeckSummary>, QString> get_deck_summaries(const QDateTime& now) override;
    std::expected<QVector<Card>, QString>        get_cards(int deckId) override;
    std::expected<Deck, QString>                 get_deck(int deckId) override;

    std::expected<int, QString>  create_deck(const Deck& deck) override;
    std::expected<void, QString> update_deck(const Deck& deck) override;
    std::expected<void, QString> delete_deck(int deckId) override;
    std::expected<bool, QString> is_deck_exists(const QString& deck_name) override;
    std::expected<int, QString> deck_id(const QString& deck_name) override;

    std::expected<void, QString> update_card(const Card& card) override;
    std::expected<void, QString> insert_cards_batch(int deckId, const QVector<Card>& cards) override;
    std::expected<void, QString> insert_card(int deckId, const Card& card) override;
    std::expected<bool, QString> is_card_exists(int deckId, const QString& card_front) override;

  signals:
    /**
     * @brief Emitted when repository data changed (UI can refresh).
     */
    void data_changed();

  private:
    Database& m_db;
};

} // namespace revise
