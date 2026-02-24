// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QtQml>          // for QML_VALUE_TYPE, Q_GADGET
#include <core/Popup.hpp> // for core popups

///@ Popup type wrapper for QML
struct PopupType {
    Q_GADGET
    QML_ELEMENT

  public:
    ///@see core::PopupType
    enum class PopupTypeEnum { INFO, CHOICE, OTHER };

    Q_ENUM(PopupTypeEnum)
};

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
    QML_ELEMENT
    QML_CONSTRUCTIBLE_VALUE

  public:
    Q_PROPERTY(QString id MEMBER id FINAL)
    Q_PROPERTY(QString text MEMBER text FINAL)
    Q_PROPERTY(bool clickable MEMBER clickable FINAL)
};

/**
 * @brief The structure describes a request for Popup. Specific intention to show popup
 *
 * This structure is a wrapper over a similar structure from core, but accessible in the UI context
 */
struct PopupRequest final : core::PopupRequest {
    Q_GADGET
    QML_ELEMENT
    QML_CONSTRUCTIBLE_VALUE

    Q_PROPERTY(PopupType::PopupTypeEnum type MEMBER type)
    Q_PROPERTY(QString message MEMBER message)
    Q_PROPERTY(QVariantList buttons MEMBER buttons)

  public:
    PopupRequest(const core::PopupRequest& other) :
        type(static_cast<PopupType::PopupTypeEnum>(other.type)), message(other.message) {
        buttons.reserve(other.buttons.size());

        for (const auto& button : buttons) {
            buttons.emplace_back(QVariant::fromValue(std::move(button)));
        }
    }

    PopupType::PopupTypeEnum type;    ///< Popup type
    QString                  message; ///< Message
    QVariantList             buttons; ///< buttons
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
    Q_INVOKABLE PopupAction(const QString& id = QString(), const QVariant& payload = QVariant()) :
        core::PopupAction{id, payload} {}

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
    QML_ELEMENT

  public:
    Q_INVOKABLE PopupResponse(const PopupAction& action = PopupAction(), const QVariant& context = QVariant()) :
        core::PopupResponse{action, context} {}

    Q_PROPERTY(PopupAction action MEMBER action)
    Q_PROPERTY(QVariant context MEMBER context)

    PopupAction action; ///< User action
};

} // namespace ui
