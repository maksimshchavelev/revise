// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Popup.hpp"  // for Popup
#include <functional> // for std::function

namespace core {

/**
 * @brief The interface through which the popup is displayed from C++ code
 */
class IPopupService {
  public:
    virtual ~IPopupService() = default;

    /**
     * @brief Method for requesting a popup message display
     * @param request Request. @see `core::PopupResponse`
     * @param callback Callback that is called when the user responds to the popup
     */
    virtual void request(const PopupRequest& request, std::function<void(const PopupResponse&)> callback) = 0;
};

} // namespace core
