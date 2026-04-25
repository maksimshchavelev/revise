// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <atomic>                        // for std::atomic_bool
#include <core/IDeckExporter.hpp>        // for core::IDeckExporter
#include <core/IDeckImporter.hpp>        // for core::IDeckImporter
#include <core/IDeckMediaStorage.hpp>    // for core::IDeckMediaStorage
#include <core/IStudyEngine.hpp>         // for core::IStudyEngine
#include <core/search/ISearchEngine.hpp> // for core::ISearchEngine
#include <core/service/IDeckService.hpp> // for core::IDeckService
#include <core/storage/IDeckStorage.hpp> // for core::IDeckStorage

namespace engine {

/// @brief `DeckService` dependencies
struct DeckServiceDeps {
    core::IDeckStorage&      deck_storage;
    core::IDeckMediaStorage& deck_media_storage;
    core::IDeckImporter&     anki_deck_importer;
    core::IDeckImporter&     revise_deck_importer;
    core::IDeckExporter&     deck_exporter;
    core::IStudyEngine&      study_engine;
    core::ISearchEngine&     search_engine;
};

/**
 * @brief Service for managing decks and cards
 */
class DeckService final : public core::IDeckService {
  public:
    DeckService(DeckServiceDeps deps);

    /// @copydoc core::IDeckService::create_deck
    QFuture<Result<void>> create_deck(const DeckDraft& deck) override;

    /// @copydoc core::IDeckService::remove_deck
    QFuture<Result<void>> remove_deck(core::Deck::id_type deck_id) override;

    /// @copydoc core::IDeckService::import_deck
    QFuture<Result<void>> import_deck(const QString& path) override;

    /// @copydoc core::IDeckService::export_deck
    QFuture<Result<void>> export_deck(core::Deck::id_type deck_id, const QString& path) override;

    /// @copydoc core::IDeckService::deck
    QFuture<Result<core::Deck>> deck(core::Deck::id_type deck_id) const override;

    /// @copydoc core::IDeckService::update_deck
    QFuture<Result<void>> update_deck(core::Deck::id_type deck_id, const DeckDraft& deck) override;

    /// @copydoc core::IDeckService::create_card
    QFuture<Result<void>> create_card(core::Deck::id_type deck_id, const CreateCardDraft& card) override;

    /// @copydoc core::IDeckService::card
    QFuture<Result<core::Card>> card(core::Card::id_type id) const override;

    /// @copydoc core::IDeckService::remove_card
    QFuture<core::IDeckService::Result<void>> remove_card(core::Card::id_type id) override;

    /// @copydoc core::IDeckService::update_card
    QFuture<Result<void>> update_card(core::Deck::id_type deck_id, const UpdateCardDraft& card) override;

    /// @copydoc core::IDeckService::deck_summaries
    QFuture<Result<QVector<core::DeckSummary>>> deck_summaries() const override;

    /// @copydoc core::IDeckService::cards
    QFuture<Result<QVector<core::Card>>> cards(core::Deck::id_type deck_id) const override;

    /// @copydoc core::IDeckService::search_cards
    QFuture<Result<QVector<core::Card>>> search_cards(const core::CardFilterChain& filters) const override;

  private:
    /// @brief Map storage error to service error
    Error from_storage_error(const core::IDeckStorage::Error& error, QString message) const;

    DeckServiceDeps m_deps;

    std::atomic_bool m_import_in_progress{false}; ///< Is import in progress now
    std::atomic_bool m_export_in_progress{false}; ///< Is export in progress now
};

} // namespace engine
