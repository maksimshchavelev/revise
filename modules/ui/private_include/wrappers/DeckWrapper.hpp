// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <../../../core/include/core/structs/Deck.hpp>
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/// @brief Wrapper over `core::Deck`, visible from QML
struct Deck final : core::Deck {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString name MEMBER name FINAL)
    Q_PROPERTY(QString description MEMBER description FINAL)
    Q_PROPERTY(int timeLimit MEMBER time_limit FINAL)
    Q_PROPERTY(int newLimit MEMBER new_limit FINAL)
    Q_PROPERTY(int reviewLimit MEMBER review_limit FINAL)
    Q_PROPERTY(int incorrectLimit MEMBER incorrect_limit FINAL)
    Q_PROPERTY(int id MEMBER id FINAL)
    Q_PROPERTY(int globalId MEMBER global_id FINAL)

  public:
    Q_INVOKABLE Deck() = default;

    Q_INVOKABLE Deck(const QString& name,
                     const QString& description,
                     const int      id,
                     const int      global_id,
                     const int      time_limit,
                     const int      new_limit,
                     const int      review_limit,
                     const int      incorrect_limit) :
        core::Deck{name, description, id, global_id, time_limit, new_limit, review_limit, incorrect_limit} {}

    explicit Deck(const core::Deck& other) : core::Deck{other} {}

    template <typename OtherDeck>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeck>>
    Deck& operator=(OtherDeck&& other) {
        Deck::operator=(std::forward<OtherDeck>(other));
        return *this;
    }

    template <typename OtherDeck>
        requires std::is_base_of_v<core::Deck, std::remove_reference_t<OtherDeck>>
    bool operator==(const OtherDeck& other) const noexcept {
        return Deck::operator==(other);
    }
};

/// @brief Wrapper over core::DeckSummary, visible from QML
struct DeckSummary final : core::DeckSummary {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(int new_cards MEMBER new_cards FINAL)
    Q_PROPERTY(int review_cards MEMBER review_cards FINAL)
    Q_PROPERTY(int incorrect_cards MEMBER incorrect_cards FINAL)
    Q_PROPERTY(Deck deck MEMBER deck FINAL)

  public:
    explicit DeckSummary(const core::DeckSummary& other) : core::DeckSummary{other} {}

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
