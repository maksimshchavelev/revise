// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/ToastServiceFactory.hpp" // for header
#include "ToastService.hpp"               // for ToastService implementation

namespace engine {

std::unique_ptr<core::IToastService> create_toast_service() {
    return std::make_unique<ToastService>();
}

} // namespace engine
