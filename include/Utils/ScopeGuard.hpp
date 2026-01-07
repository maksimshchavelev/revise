// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <functional> // for std::functional

namespace revise {

/**
 * @brief A helper class based on the RAII principle for performing certain
 * actions when creating/destroying an object.
 */
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> on_enter,
                        std::function<void()> on_exit);

    ~ScopeGuard();

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator = (const ScopeGuard&) = delete;
    ScopeGuard& operator = (ScopeGuard&&) = delete;

private:
    std::function<void()> m_on_exit;
};

}
