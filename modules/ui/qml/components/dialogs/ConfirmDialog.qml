// ConfirmDialog.qml

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype ConfirmDialog
    \brief A modal confirmation dialog with an optional "don't ask again" checkbox.
    \inqmlmodule Revise
    \inherits Dialog
    \since 1.0

    ConfirmDialog shows a title, a message, and a pair of cancel/confirm
    buttons, and optionally a "don't ask again" checkbox. It is modal,
    centered over the window, and closes on Escape or on a click outside the
    dialog in addition to the explicit buttons. The confirm button receives
    initial keyboard focus, so pressing Enter activates it as soon as the
    dialog opens.

    The dialog fades in and scales up slightly on open, and collapses by
    scaling down on close.

    \section1 Example Usage

    \qml
    Revise.ConfirmDialog {
        titleText: qsTr("Удалить колоду?")
        messageText: qsTr("Это действие нельзя отменить.")
        confirmButtonText: qsTr("Удалить")
        onAccepted: Revise.DeckService.remove_deck(deckId)
        onDontAskAgainChanged: (checked) => settings.skipDeleteConfirmation = checked
    }
    \endqml
*/
Dialog {
    id: root

    /*!
        \qmlproperty string ConfirmDialog::titleText
        \brief The title shown in the dialog header.
        \default qsTr("Подтверждение")
    */
    property string titleText: qsTr("Подтверждение")

    /*!
        \qmlproperty string ConfirmDialog::messageText
        \brief The body text shown below the title.

        The text is word-wrapped and rendered with
        \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor}.
    */
    property string messageText

    /*!
        \qmlproperty string ConfirmDialog::confirmButtonText
        \brief The text of the confirm button.

        The button uses
        \l {Revise.Theme::primaryColor}{Revise.Theme.primaryColor} as its
        background and calls \l accept() when clicked.
    */
    property string confirmButtonText

    /*!
        \qmlproperty string ConfirmDialog::cancelButtonText
        \brief The text of the cancel button.
        \default qsTr("Отмена")

        The button calls \l reject() when clicked.
    */
    property string cancelButtonText: qsTr("Отмена")

    /*!
        \qmlproperty bool ConfirmDialog::dontAskAgainValue
        \brief Whether the "don't ask again" checkbox is checked.

        This property is an alias to the internal
        \l {Revise.LabeledCheckBox::checked}{Revise.LabeledCheckBox.checked},
        so its state can be read when the user confirms the dialog.
    */
    property alias dontAskAgainValue: dontAskAgainCheckBox.checked

    /*!
        \qmlsignal ConfirmDialog::dontAskAgainChanged(bool checked)
        \brief Emitted when the "don't ask again" checkbox is toggled.

        The \a checked argument contains the new state of the checkbox. The
        signal is forwarded from the internal
        \l {Revise.LabeledCheckBox::changed}{Revise.LabeledCheckBox.changed}.
    */
    signal dontAskAgainChanged(bool checked);

    anchors.centerIn: Overlay.overlay
    width: Math.min(400, Overlay.overlay ? Overlay.overlay.width * 0.9 : 400)
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
            from: 0.75
            to: 1.0
            duration: 280
            easing.type: Easing.OutBack
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "scale"
            from: 1.0
            to: 0.5
            duration: 160
            easing.type: Easing.InQuint
        }
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.4)
        Behavior on opacity {
            NumberAnimation { duration: 140 }
        }
    }

    background: Rectangle {
        radius: 12
        color: Revise.Theme.surfaceColor
        layer.enabled: true
        layer.effect: Revise.Shadow {}
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

    footer: Item {
        implicitHeight: footerLayout.implicitHeight + 30

        Column {
            id: footerLayout

            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Revise.HorizontalDivider {
                width: parent.width
            }

            RowLayout {
                width: parent.width

                Revise.LabeledCheckBox {
                    id: dontAskAgainCheckBox
                    Layout.fillWidth: true
                    text: qsTr("Больше не спрашивать")
                    color: Revise.Theme.secondaryForegroundColor
                    onChanged: function(checked) {
                        root.dontAskAgainChanged(checked);
                    }
                }

                Revise.Button {
                    text: root.cancelButtonText
                    onClicked: root.reject()
                }

                Revise.Button {
                    text: root.confirmButtonText
                    backgroundItem.color: Revise.Theme.primaryColor
                    onClicked: root.accept()
                    focus: true
                }
            }
        }
    }
}
