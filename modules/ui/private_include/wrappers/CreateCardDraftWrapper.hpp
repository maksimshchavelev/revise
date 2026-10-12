// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT
#include <core/service/IDeckService.hpp>      // for core::IDeckService::CreateCardDraft
#include <type_traits>                        // for std::is_base_of, std::remove_reference

namespace ui {

/// @brief Wrapper over `core::IDeckService::CreateCardDraft`, visible from QML
struct CreateCardDraft final : core::IDeckService::CreateCardDraft {
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString front MEMBER front FINAL)
    Q_PROPERTY(QString back MEMBER back FINAL)

  public:
    Q_INVOKABLE CreateCardDraft() = default;

    Q_INVOKABLE CreateCardDraft(const QString& front, const QString& back) : core::IDeckService::CreateCardDraft{front, back} {}

    explicit CreateCardDraft(const core::IDeckService::CreateCardDraft& other) : core::IDeckService::CreateCardDraft{other} {}

    template <typename OtherCard>
        requires std::is_base_of_v<core::IDeckService::CreateCardDraft, std::remove_reference_t<OtherCard>>
    CreateCardDraft& operator=(OtherCard&& other) {
        core::IDeckService::CreateCardDraft::operator=(std::forward<OtherCard>(other));
        return *this;
    }

    template <typename OtherCard>
        requires std::is_base_of_v<core::IDeckService::CreateCardDraft, std::remove_reference_t<OtherCard>>
    bool operator==(const OtherCard& other) const noexcept {
        return core::IDeckService::CreateCardDraft::operator==(other);
    }
};

} // namespace ui
