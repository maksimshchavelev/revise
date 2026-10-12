// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/service/IDeckService.hpp>      // for core::IDeckService::DeckDraft
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/// @brief Wrapper over `core::IDeckService::DeckDraft`, visible from QML
struct DeckDraft final : core::IDeckService::DeckDraft {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString name MEMBER name FINAL)
    Q_PROPERTY(QString description MEMBER description FINAL)
    Q_PROPERTY(int time_limit MEMBER time_limit FINAL)
    Q_PROPERTY(int new_limit MEMBER new_limit FINAL)
    Q_PROPERTY(int review_limit MEMBER review_limit FINAL)
    Q_PROPERTY(int incorrect_limit MEMBER incorrect_limit FINAL)

  public:
    Q_INVOKABLE DeckDraft() = default;

    Q_INVOKABLE DeckDraft(const QString& name,
                          const QString& description,
                          const int      time_limit,
                          const int      new_limit,
                          const int      review_limit,
                          const int      incorrect_limit) :
        core::IDeckService::DeckDraft{name, description, new_limit, review_limit, incorrect_limit, time_limit} {}

    explicit DeckDraft(const core::IDeckService::DeckDraft& other) : core::IDeckService::DeckDraft{other} {}

    template <typename OtherDeck>
        requires std::is_base_of_v<core::IDeckService::DeckDraft, std::remove_reference_t<OtherDeck>>
    DeckDraft& operator=(OtherDeck&& other) {
        core::IDeckService::DeckDraft::operator=(std::forward<OtherDeck>(other));
        return *this;
    }

    template <typename OtherDeck>
        requires std::is_base_of_v<core::IDeckService::DeckDraft, std::remove_reference_t<OtherDeck>>
    bool operator==(const OtherDeck& other) const noexcept {
        return core::IDeckService::DeckDraft::operator==(other);
    }
};

} // namespace ui
