// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/service/IDeckService.hpp>      // for core::IDeckService::UpdateCardDraft
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/// @brief Wrapper over `core::IDeckService::UpdateCardDraft`, visible from QML
struct UpdateCardDraft final : core::IDeckService::UpdateCardDraft {
    Q_GADGET
    QML_VALUE_TYPE(updateCardDraft)

    Q_PROPERTY(QString front READ front WRITE set_front FINAL)
    Q_PROPERTY(QString back READ back WRITE set_back FINAL)
    Q_PROPERTY(float difficulty READ difficulty WRITE set_difficulty FINAL)

  public:
    Q_INVOKABLE UpdateCardDraft() = default;

    Q_INVOKABLE UpdateCardDraft(const QString& front, const QString& back, const float difficulty) :
        core::IDeckService::UpdateCardDraft{front, back, difficulty} {}

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

private:
    /// @brief Get front of the draft
    QString front() const {
        return core::IDeckService::UpdateCardDraft::front.value_or(QString());
    }

    /// @brief Set front of the draft
    void set_front(QString front) {
        core::IDeckService::UpdateCardDraft::front.emplace(std::move(front));
    }

    /// @brief Get back of the draft
    QString back() const {
        return core::IDeckService::UpdateCardDraft::back.value_or(QString());
    }

    /// @brief Set back of the draft
    void set_back(QString back) {
        core::IDeckService::UpdateCardDraft::back.emplace(std::move(back));
    }

    /// @brief Get difficulty of the draft
    float difficulty() const {
        return core::IDeckService::UpdateCardDraft::difficulty.value_or(0.0f);
    }

    /// @brief Set difficulty of the draft
    void set_difficulty(const float difficulty) {
        core::IDeckService::UpdateCardDraft::difficulty = difficulty;
    }
};

} // namespace ui
