// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QQueue>                 // for QQueue
#include <core/IPopupService.hpp> // for core::IPopupService

namespace engine {

/// @brief PopupService implementation
class PopupService final : public core::IPopupService {
  public:
    /// @copydoc core::IPopupService::request
    void request(const core::PopupRequest& request, std::function<void(const core::PopupResponse&)> callback) override;

    /// @copydoc core::IPopupService::response
    void response(const core::PopupResponse& response) override;

  private:
    /// @brief Structure storing the request and callback
    struct Request {
        core::PopupRequest                              request;  ///< Request
        std::function<void(const core::PopupResponse&)> callback; ///< Callback
    };

    QQueue<Request> m_requests; ///< Requests
};

} // namespace engine
