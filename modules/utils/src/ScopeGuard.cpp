// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "utils/ScopeGuard.hpp" // for ScopeGuard header

namespace utils {

// Public method
ScopeGuard::ScopeGuard(std::function<void()> on_enter, std::function<void()> on_exit) : m_on_exit(std::move(on_exit)) {
    on_enter();
}

ScopeGuard::~ScopeGuard() {
    m_on_exit();
}

} // namespace utils
