// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QNetworkAccessManager> // for QNetworkAccessManager
#include <QThread>               // for QThread
#include <concepts>              // for concepts

namespace network {

/// @brief The context for executing network requests via `QNetworkAccessManager`
///
/// The class provides a way to call a function in the same thread as the internal `QNetworkAccessManager` and pass a reference to
/// it to that function
class RequestExecutionContext {
  public:
    RequestExecutionContext();
    ~RequestExecutionContext();

    /// @brief Execute a blocking function in the `QNetworkAccessManager` thread, a reference to which will be passed to this
    /// function
    template <typename Fn>
    std::invoke_result_t<Fn, QNetworkAccessManager&> exec(Fn&& fn)
        requires std::invocable<Fn, QNetworkAccessManager&>
    {
        using InvokeResult = std::invoke_result_t<Fn, QNetworkAccessManager&>;

        if (m_thread.isCurrentThread()) /* we are in the m_manager's thread */ {
            return std::invoke(std::forward<Fn>(fn), m_manager);
        }

        if constexpr (std::is_void_v<InvokeResult>) {
            QMetaObject::invokeMethod(
                &m_manager, [this, fn = std::forward<Fn>(fn)] { fn(m_manager); }, Qt::BlockingQueuedConnection);
            return;
        } else {
            std::optional<InvokeResult> result;
            QMetaObject::invokeMethod(
                &m_manager,
                [this, fn = std::forward<Fn>(fn), &result] { result.emplace(fn(m_manager)); },
                Qt::BlockingQueuedConnection);
            return std::move(*result);
        }
    }

  private:
    QThread               m_thread;  ///< Execution thread
    QNetworkAccessManager m_manager; ///< Network access manager
};

} // namespace network
