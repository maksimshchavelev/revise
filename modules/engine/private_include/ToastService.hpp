// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <../../core/include/core/service/IToastService.hpp>
#include <QQueue> // for QQueue

namespace engine {

/// @brief Implementation of `core::IToastService`
class ToastService : public core::IToastService {
  public:
    /// @copydoc core::IToastService::request
    void request(core::Toast toast) override;

    /// @copydoc core::IToastService::close_last
    void close_last() override;

  private:
    QQueue<core::Toast> m_requests; ///< Toasts queue
};

} // namespace engine
