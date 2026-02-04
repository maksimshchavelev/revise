// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/PopupServiceFactory.hpp" // for PopupServiceFactory header
#include "PopupService.hpp"           // for PopupServices

namespace ui {

std::unique_ptr<core::IPopupService> create_popup_service() {
    return std::make_unique<PopupService>();
}

} // namespace ui