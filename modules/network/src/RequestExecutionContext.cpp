// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "network/RequestExecutionContext.hpp" // for header

namespace network {

RequestExecutionContext::RequestExecutionContext() {
    m_manager.moveToThread(&m_thread);
    m_thread.start();
}

RequestExecutionContext::~RequestExecutionContext() {
    m_thread.quit();
    m_thread.wait();
}

} // namespace network
