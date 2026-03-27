// Add formula popup (inline or display)

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Revise as Revise

Revise.PopupBase {
    id: root

    signal addInline
    signal addDisplay

    Revise.Button {
        text: qsTr("Встроенная формула")
        Layout.preferredWidth: parent.width * 0.85
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 20
        onClicked: {
            root.close()
            root.addInline()
        }
    }

    Revise.Button {
        text: qsTr("Блок с формулой")
        Layout.preferredWidth: parent.width * 0.85
        Layout.alignment: Qt.AlignHCenter
        Layout.bottomMargin: 20
        onClicked: {
            root.close()
            root.addDisplay()
        }
    }

    function open() {
        root.visible = true
    }

    function close() {
        root.visible = false
    }
}
