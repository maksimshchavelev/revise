// Dialog.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype Dialog
    \brief A base dialog with a title and message content.
    \inqmlmodule Revise
    \inherits QtQuick.Controls.Dialog
    \since 1.0

    Dialog is a base dialog that displays a title and a message as its content item.
    It is intended to be used as a foundation for building other dialog messages.
    The \l contentItem can be overridden and a \l footer can be provided to customize the dialog.
*/
Dialog {
    id: root

    /*!
        \qmlproperty string ConfirmDialog::titleText
        \brief The title shown in the dialog header.
    */
    property string titleText

    /*!
        \qmlproperty string ConfirmDialog::messageText
        \brief The body text shown below the title.

        The text is word-wrapped and rendered with
        \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor}.
    */
    property string messageText

    /*!
        \qmlproperty real ConfirmDialog::effectiveScale
        \brief The effective visual scale of the dialog.

        This value follows \c Revise.SettingsService.uiScale but is reduced
        when necessary to keep the dialog within the available overlay area.
        It is read-only.
    */
    readonly property real effectiveScale: {
        const requestedScale = Math.max(0.01, Revise.SettingsService.uiScale)

        const overlay = Overlay.overlay

        if (!overlay || width <= 0 || height <= 0)
            return requestedScale

        const maxWidthScale = Math.max(0.01, (overlay.width - 32) / width)
        const maxHeightScale = Math.max(0.01, (overlay.height - 32) / height)

        return Math.min(requestedScale, maxWidthScale, maxHeightScale)
    }

    anchors.centerIn: Overlay.overlay
    transformOrigin: Item.Center
    margins: 16

    width: Math.min(400, Overlay.overlay ? Overlay.overlay.width * 0.9 : 400)
    scale: effectiveScale
    modal: true

    padding: 0
    spacing: 20

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: 220
            easing.type: Easing.OutCubic
        }

        NumberAnimation {
            property: "scale"
            from: root.effectiveScale * 0.75
            to: root.effectiveScale
            duration: 280
            easing.type: Easing.OutBack
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "scale"
            from: root.effectiveScale
            to: root.effectiveScale * 0.5
            duration: 160
            easing.type: Easing.InQuint
        }
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.4)

        Behavior on opacity {
            NumberAnimation {
                duration: 140
            }
        }
    }

    background: Rectangle {
        radius: 12
        color: Revise.Theme.surfaceColor

        layer.effect: Revise.Shadow
        {
        }
    }

    header: Item {
        implicitHeight: titleLabel.implicitHeight

        Revise.Text {
            id: titleLabel

            anchors.fill: parent
            anchors.margins: 12

            text: root.titleText
            font.pixelSize: Revise.Theme.fontSizeMedium
            font.bold: true

            color: Revise.Theme.foregroundColor
            wrapMode: Text.WordWrap
        }
    }

    contentItem: Item {
        implicitHeight: messageLabel.implicitHeight

        Revise.Text {
            id: messageLabel

            anchors.fill: parent
            anchors.margins: 12

            text: root.messageText
            color: Revise.Theme.secondaryForegroundColor
            wrapMode: Text.WordWrap
        }
    }
}
