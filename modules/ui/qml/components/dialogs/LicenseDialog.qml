// LicenseDialog.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype LicenseDialog
    \brief An optional dialog for viewing license text.
    \inqmlmodule Revise
    \inherits Revise.Dialog
    \since 1.0

    LicenseDialog is an optional dialog that displays license text in a scrollable, copyable view.
    It is based on Revise.Dialog and overrides the \l contentItem to provide a Flickable with CopyableText, and the \l footer to include an OK button that accepts the dialog.
    The license text is taken from the \l messageText property inherited from Revise.Dialog.
*/
Revise.Dialog {
    id: root

    width: Math.min(720, Overlay.overlay ? Overlay.overlay.width * 0.9 : 720)

    contentItem: Item {
        implicitWidth: 480
        implicitHeight: Math.min(licenseLabel.implicitHeight + 24, (Overlay.overlay ? Overlay.overlay.height : 600) * 0.75)

        Revise.Flickable {
            anchors.fill: parent
            clip: true
            contentWidth: width
            contentHeight: licenseLabel.implicitHeight + 24

            Revise.CopyableText {
                id: licenseLabel

                width: parent.width - 24
                x: 12
                y: 12

                text: root.messageText
                color: Revise.Theme.secondaryForegroundColor
                wrapMode: Text.Wrap
            }
        }
    }

    footer: Item {
        implicitHeight: okButton.implicitHeight + 30

        Revise.Button {
            id: okButton
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 12
            text: qsTr("Понятно")
            backgroundItem.color: Revise.Theme.primaryColor
            onClicked: root.accept()
            focus: true
        }
    }
}
