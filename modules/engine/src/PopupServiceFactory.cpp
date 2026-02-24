// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/PopupServiceFactory.hpp" // for header
#include "PopupService.hpp"               // for PopupService

namespace engine {

std::unique_ptr<core::IPopupService> create_popup_service() {
    return std::make_unique<PopupService>();
}

} // namespace engine
