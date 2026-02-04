// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Popup.hpp"              // for popup structures
#include <QObject>                // for QObject
#include <QQueue>                 // for QQueue
#include <core/IPopupService.hpp> // for IPopupService

namespace ui {

class PopupService final : public QObject, public core::IPopupService {
Q_OBJECT

  public:
    explicit PopupService(QObject* parent = nullptr);

    /**
     * @see `core::IPopupService` for details
     */
    void request(const core::PopupRequest& request, std::function<void(const core::PopupResponse&)> callback) override;

    /**
     * @brief Called by the UI code to respond to the popup
     * @param response `ui::PopupResponse`
     */
    void Q_INVOKABLE response(const PopupResponse& response);

  signals:
    /**
     * @brief This signal is emitted when a request to display a popup is received.
     */
    void requested(const PopupRequest& request);

    /**
     * @brief This signal is emitted when a response to the displayed popup is received.
     */
    void responsed(const PopupResponse& response);

  private:
    /**
     * @brief This structure describes the current request at a specific point in time, as well as the callback that
     * will be called when a response is received.
     */
    struct Pending {
        PopupRequest request; ///< Current request
        std::function<void(const core::PopupResponse&)>
            callback; ///< Called when a response to popup is received. Contains
        ///< a callback for the popup that is currently displayed.
    };

    QQueue<Pending> m_requests; ///< Queue messages so that they are not displayed simultaneously
};

} // namespace ui