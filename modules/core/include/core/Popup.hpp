// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>  // for QString
#include <QVariant> // for QVariant
#include <QVector>  // for QVector

namespace core {

/**
 * @brief The structure describes the button displayed on the popup. There can be several buttons.
 */
struct PopupButton {
    QString id;        ///< ID of button, for example, `ok` or `cancel`. The ID is chosen arbitrarily.
    QString text;      ///< The text that appears on the button
    bool    clickable; ///< Is the button clickable?
};

/**
 * @brief The structure describes a request for Popup. Specific intention to show popup
 */
struct PopupRequest {
    QString type; ///< Request type. This is a view-selection key: it helps the UI choose which QML component or
                  ///< template to display (“confirm”, “deckActions”, ‘error’, “textInput”, etc.). This is not a
                  ///< specific request identifier, but a popup class.
    QString              message; ///< The message that will be displayed on the popup
    QVector<PopupButton> buttons; ///< Buttons in the popup. @see `core::PopupButton`
    QVariant             context; ///< Arbitrary context passed along with the popup request
};

/**
 * @brief This structure describes the user's response to the popup. An event that has already occurred. The result of
 * displaying the popup.
 */
struct PopupAction {
    QString  id;      ///< User action ID
    QVariant payload; ///< Payload of the response
};

/**
 * @brief This structure describes the user's response to a popup.
 */
struct PopupResponse {
    PopupAction action;  ///< User response
    QVariant    context; ///< Popup context
};

} // namespace core