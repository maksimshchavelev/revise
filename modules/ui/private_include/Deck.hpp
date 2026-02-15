// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for Q_GADGET
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/Deck.hpp>                      // for core::Deck and core::DeckSummary
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/**
 * @brief Wrapper over core::Deck, visible from QML
 */
struct Deck final : core::Deck {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString name MEMBER name FINAL)
    Q_PROPERTY(QString description MEMBER description FINAL)
    Q_PROPERTY(int time_limit MEMBER time_limit FINAL)
    Q_PROPERTY(int new_limit MEMBER new_limit FINAL)
    Q_PROPERTY(int consolidate_limit MEMBER consolidate_limit FINAL)
    Q_PROPERTY(int incorrect_limit MEMBER incorrect_limit FINAL)
    Q_PROPERTY(int id MEMBER id FINAL)
    Q_PROPERTY(int global_id MEMBER global_id FINAL)

  public:
    template <typename OtherDeck>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeck>>
    Deck& operator=(OtherDeck&& other) {
        core::Deck::operator=(std::forward<OtherDeck>(other));
        return *this;
    }

    template <typename OtherDeck>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeck>>
    bool operator==(const OtherDeck& other) const noexcept {
        return core::Deck::operator==(other);
    }
};

/**
 * @brief Wrapper over core::DeckSummary, visible from QML
 */
struct DeckSummary final : core::DeckSummary {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(int new_cards MEMBER new_cards FINAL)
    Q_PROPERTY(int consolidate_cards MEMBER consolidate_cards FINAL)
    Q_PROPERTY(int incorrect_cards MEMBER incorrect_cards FINAL)
    Q_PROPERTY(Deck deck MEMBER deck FINAL)

  public:
    template <typename OtherDeckSummary>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeckSummary>>
    DeckSummary& operator=(OtherDeckSummary&& other) {
        core::DeckSummary::operator=(std::forward<OtherDeckSummary>(other));
        return *this;
    }

    template <typename OtherDeckSummary>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeckSummary>>
    bool operator==(const OtherDeckSummary& other) const noexcept {
        return core::DeckSummary::operator==(other);
    }
};


} // namespace ui
