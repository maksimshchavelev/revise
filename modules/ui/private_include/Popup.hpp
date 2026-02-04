// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQml>          // for QML_VALUE_TYPE, Q_GADGET
#include <core/Popup.hpp> // for core popups

namespace ui {

/**
 * @brief The structure describes the button displayed on the popup. There can be several buttons.
 *
 * This structure is a wrapper over a similar structure from core, but accessible in the UI context.
 *
 * @see `core::PopupButton` for details
 */
struct PopupButton final : core::PopupButton {
    Q_GADGET
    QML_VALUE_TYPE(PopupButton)

  public:
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString text MEMBER text)
    Q_PROPERTY(bool clickable MEMBER clickable)
};

/**
 * @brief The structure describes a request for Popup. Specific intention to show popup
 *
 * This structure is a wrapper over a similar structure from core, but accessible in the UI context
 */
struct PopupRequest final : core::PopupRequest {
    Q_GADGET
    QML_VALUE_TYPE(PopupRequest)

  public:
    Q_PROPERTY(QString type MEMBER type)
    Q_PROPERTY(QString message MEMBER message)
    Q_PROPERTY(QVariantList buttons MEMBER buttons)
    Q_PROPERTY(QVariant context MEMBER context)

    QVariantList buttons; ///< buttons
};

/**
 * @brief This structure describes the user's response to the popup. An event that has already occurred. The result of
 * displaying the popup.
 *
 * This structure is a wrapper over a similar structure from core, but accessible in the UI context
 */
struct PopupAction final : core::PopupAction {
    Q_GADGET
    QML_VALUE_TYPE(PopupAction)

  public:
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QVariant payload MEMBER payload)

    bool operator==(const PopupAction& other) const {
        return id == other.id && payload == other.payload;
    }
};

/**
 * @brief This structure describes the user's response to a popup.
 *
 * This structure is a wrapper over a similar structure from core, but accessible in the UI context
 */
struct PopupResponse final : core::PopupResponse {
    Q_GADGET
    QML_VALUE_TYPE(PopupResponse)

  public:
    Q_PROPERTY(PopupAction action MEMBER action)
    Q_PROPERTY(QVariant context MEMBER context)

    PopupAction action; ///< User action
};

} // namespace ui

Q_DECLARE_METATYPE(ui::PopupButton)
Q_DECLARE_METATYPE(ui::PopupRequest)
Q_DECLARE_METATYPE(ui::PopupResponse)
Q_DECLARE_METATYPE(ui::PopupAction)
