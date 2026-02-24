// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Notifiable.hpp" // for Notifiable
#include "Popup.hpp"      // for Popup
#include <functional>     // for std::function

namespace core {

/**
 * @brief The interface through which the popup is displayed from C++ code
 */
class IPopupService : public Notifiable {
  public:
    EVENT(requested, PopupRequest request) ///< Popup requested

    virtual ~IPopupService() = default;

    /**
     * @brief Method for requesting a popup message display
     * @param request Request. @see `core::PopupResponse`
     * @param callback Callback that is called when the user responds to the popup
     * @note The `requested` event is only emitted when the queue reaches the displayed popup.
     */
    virtual void request(const PopupRequest& request, std::function<void(const PopupResponse&)> callback) = 0;

    /**
     * @brief Response to the popup that is currently displayed
     *
     * If there are several popups in the queue, the next one will be displayed.
     *
     * @param response Response
     */
    virtual void response(const PopupResponse& response) = 0;
};

} // namespace core
