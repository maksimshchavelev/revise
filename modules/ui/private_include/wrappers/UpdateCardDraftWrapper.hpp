// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/service/IDeckService.hpp>      // for core::IDeckService::UpdateCardDraft
#include <type_traits>                        // for std::is_base_of, std::remove_reference
#include <wrappers/CardWrapper.hpp>           // for ui::Card wrapper

namespace ui {

/// @brief Wrapper over `core::IDeckService::UpdateCardDraft`, visible from QML
struct UpdateCardDraft final : core::IDeckService::UpdateCardDraft {
    Q_GADGET
    QML_CONSTRUCTIBLE_VALUE
    QML_VALUE_TYPE(updateCardDraft)

    Q_PROPERTY(QString front READ front WRITE set_front FINAL)
    Q_PROPERTY(QString back READ back WRITE set_back FINAL)
    Q_PROPERTY(float difficulty READ difficulty WRITE set_difficulty FINAL)
    Q_PROPERTY(int id READ id WRITE set_id FINAL)

  public:
    /// @brief Construct `UpdateCardDraft` with default values
    Q_INVOKABLE UpdateCardDraft() = default;

    /// @brief Create `UpdateCardDraft` with id
    /// @param id Local Card ID
    explicit Q_INVOKABLE UpdateCardDraft(const int id) {
        set_id(id);
    }

    /// @brief Create `UpdateCardDraft` from `ui::Card`
    explicit Q_INVOKABLE UpdateCardDraft(Card card) {
        set_id(card.id());
        set_front(card.front());
        set_back(card.back());
        set_difficulty(card.difficulty());
    }

    explicit UpdateCardDraft(const core::IDeckService::UpdateCardDraft& other) : core::IDeckService::UpdateCardDraft{other} {}

    template <typename OtherCard>
        requires std::is_base_of_v<core::IDeckService::UpdateCardDraft, std::remove_reference_t<OtherCard>>
    UpdateCardDraft& operator=(OtherCard&& other) {
        core::IDeckService::UpdateCardDraft::operator=(std::forward<OtherCard>(other));
        return *this;
    }

    template <typename OtherCard>
        requires std::is_base_of_v<core::IDeckService::UpdateCardDraft, std::remove_reference_t<OtherCard>>
    bool operator==(const OtherCard& other) const noexcept {
        return core::IDeckService::UpdateCardDraft::operator==(other);
    }

    /// @brief Get front of the draft
    [[nodiscard]] QString front() const {
        return core::IDeckService::UpdateCardDraft::front.value_or(QString());
    }

    /// @brief Set front of the draft
    void set_front(QString front) {
        core::IDeckService::UpdateCardDraft::front.emplace(std::move(front));
    }

    /// @brief Get back of the draft
    [[nodiscard]] QString back() const {
        return core::IDeckService::UpdateCardDraft::back.value_or(QString());
    }

    /// @brief Set back of the draft
    void set_back(QString back) {
        core::IDeckService::UpdateCardDraft::back.emplace(std::move(back));
    }

    /// @brief Get difficulty of the draft
    [[nodiscard]] float difficulty() const noexcept {
        return core::IDeckService::UpdateCardDraft::difficulty.value_or(0.0f);
    }

    /// @brief Set difficulty of the draft
    void set_difficulty(const float difficulty) noexcept {
        core::IDeckService::UpdateCardDraft::difficulty = difficulty;
    }

    /// @brief Get id of the draft
    [[nodiscard]] int id() const noexcept {
        return core::IDeckService::UpdateCardDraft::id;
    }

    /// @brief Set id of the draft
    void set_id(const int id) noexcept {
        core::IDeckService::UpdateCardDraft::id = id;
    }
};

} // namespace ui
