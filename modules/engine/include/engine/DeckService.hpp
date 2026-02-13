// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <atomic>                     // for std::atomic_bool
#include <core/IDeckExporter.hpp>     // for core::IDeckExporter
#include <core/IDeckImporter.hpp>     // for core::IDeckImporter
#include <core/IDeckMediaStorage.hpp> // for core::IDeckMediaStorage
#include <core/IDeckService.hpp>      // for core::IDeckService
#include <core/IDeckStorage.hpp>      // for core::IDeckStorage

namespace engine {

struct DeckServiceDeps {
    core::IDeckStorage&      deck_storage;
    core::IDeckMediaStorage& deck_media_storage;
    core::IDeckImporter&     anki_deck_importer;
    core::IDeckImporter&     revise_deck_importer;
    core::IDeckExporter&     deck_exporter;
};

/**
 * @brief Service for managing decks and cards
 */
class DeckService final : public core::IDeckService {
  public:
    DeckService(DeckServiceDeps deps);

    /// @copydoc core::IDeckService::create_deck
    std::expected<void, QString> create_deck(const core::Deck& deck) override;

    /// @copydoc core::IDeckService::remove_deck
    std::expected<void, QString> remove_deck(int deck_id) override;

    /// @copydoc core::IDeckService::import_deck_async
    void import_deck_async(const QString& path) override;

    /// @copydoc core::IDeckService::export_deck_async
    void export_deck_async(int deck_id, const QString& path) override;

    /// @copydoc core::IDeckService::deck
    std::expected<core::Deck, QString> deck(int deck_id) override;

    /// @copydoc core::IDeckService::update_deck
    std::expected<void, QString> update_deck(const core::Deck& deck) override;

    /// @copydoc core::IDeckService::create_card
    std::expected<void, QString> create_card(const core::Card& card) override;

    /// @copydoc core::IDeckService::card
    std::expected<core::Card, QString> card(int id) override;

    /// @copydoc core::IDeckService::remove_card
    std::expected<void, QString> remove_card(int id) override;

    /// @copydoc core::IDeckService::update_card
    std::expected<void, QString> update_card(const core::Card& card) override;

  private:
    DeckServiceDeps m_deps;

    std::atomic_bool m_import_in_progress{false}; ///< Is import in progress now
    std::atomic_bool m_export_in_progress{false}; ///< Is export in progress now
};

} // namespace engine
