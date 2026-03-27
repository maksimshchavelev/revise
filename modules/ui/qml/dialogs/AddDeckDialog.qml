// This dialog is needed to add a deck.

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Revise as Revise

Revise.PopupBase {
    id: root

    signal importClicked() // Called when the user clicks the deck import button
    signal createClicked() // Called when the user clicks the deck create button

    Revise.Button {
        text: qsTr("Импортировать")
        Layout.preferredWidth: parent.width * 0.85
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 20
        onClicked: {
            root.close()
            root.importClicked()
        }
    }

    Revise.Button {
        text: qsTr("Создать")
        Layout.preferredWidth: parent.width * 0.85
        Layout.alignment: Qt.AlignHCenter
        Layout.bottomMargin: 20
        onClicked: {
            root.close()
            root.createClicked()
        }
    }

    function open() {
        _openBase()
    }

    function close() {
        _closeBase()
    }
}
