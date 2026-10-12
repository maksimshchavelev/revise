// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/service/IDeckService.hpp>      // for core::IDeckService::UpdateCardDraft
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/// @brief Wrapper over `core::IDeckService::UpdateCardDraft`, visible from QML
struct UpdateCardDraft final : core::IDeckService::UpdateCardDraft {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString front MEMBER front FINAL)
    Q_PROPERTY(QString back MEMBER back FINAL)
    Q_PROPERTY(float difficulty MEMBER difficulty FINAL)

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
};

} // namespace ui
