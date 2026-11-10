
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

    The dialog is intended to be driven through \l requestOpen() instead of
    \l open(). When \l suppressed is \c true, \l requestOpen() emits
    \l accepted without showing the dialog, which lets callers skip the
    prompt based on a persistent setting. The \l dontAskAgain checkbox is
    initialized from \l suppressed each time the dialog opens.

    The dialog uses \c Revise.SettingsService.uiScale to scale its entire
    visual representation, including the background and all child items.
    The effective scale is automatically reduced when necessary to keep
    the dialog within the available overlay area.

    The dialog fades in and scales up slightly on open, and collapses by
    scaling down on close.

    \section1 Example Usage

    \qml
    Revise.ConfirmDialog {
        id: deleteDialog

        titleText: qsTr("Delete deck?")
        messageText: qsTr("This action cannot be undone.")
        confirmButtonText: qsTr("Delete")

        suppressed: !Revise.SettingsService.askRemoveDeck

        onOpened: dontAskAgain = suppressed

        onAccepted: {
            Revise.SettingsService.askRemoveDeck = !dontAskAgain
            Revise.DeckService.remove_deck(deckId)
        }
    }
    \endqml

    Open the dialog through \l requestOpen():

    \qml
    deleteDialog.requestOpen()
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
        \qmlproperty bool ConfirmDialog::dontAskAgain
        \brief Whether the "don't ask again" checkbox is checked.

        This property is an alias to the internal
        \l {Revise.LabeledCheckBox::checked}{Revise.LabeledCheckBox.checked}.
        It is initialized from \l suppressed when the dialog opens, and its
        value can be read in the \l accepted handler to persist the user's
        choice. ConfirmDialog does not emit a signal when the checkbox is
        toggled; consumers should read this property from \c onAccepted
        instead.

        The default value is \c false.
    */
    property alias dontAskAgain: dontAskAgainCheckBox.checked

    /*!
        \qmlproperty bool ConfirmDialog::showDontAskAgainOption
        \brief Should a checkbox be displayed asking whether to stop showing this dialog?

        The default value is \c true.
    */
    property bool showDontAskAgainOption: true

    /*!
        \qmlproperty bool ConfirmDialog::suppressed
        \brief Whether the prompt should be skipped entirely.

        When this property is \c true, \l requestOpen() emits \l accepted
        without showing the dialog. When it is \c false, \l requestOpen()
        opens the dialog normally. Typically bound to the inverse of a
        persistent "ask before" setting, and copied into
        \l dontAskAgain when the dialog opens.

        The default value is \c false.
    */
    property bool suppressed: false

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

    /*!
        \qmlmethod void ConfirmDialog::requestOpen()
        \brief Opens the dialog, or emits \l accepted immediately if
        \l suppressed is \c true.

        Use this method instead of \l open() when the dialog is triggered by
        a user action and the prompt may have been suppressed by a
        persistent setting. When \l suppressed is \c true, the dialog is not
        shown and \l accepted is emitted directly, so the caller's
        \c onAccepted handler runs exactly as if the user had confirmed.
    */
    function requestOpen() {
        dontAskAgain = suppressed

        if (suppressed)
            accepted()
        else
            open()
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
                    visible: root.showDontAskAgainOption

                    text: qsTr("Больше не спрашивать")
                    color: Revise.Theme.secondaryForegroundColor

                    onChanged: function (checked) {
                        dontAskAgainCheckBox.checked = checked
                    }
                }

                Revise.HorizontalSpacer {
                    visible: !root.showDontAskAgainOption
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
