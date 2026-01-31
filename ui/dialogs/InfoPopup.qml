// A pop-up notification that displays plain text

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Revise

PopupBase {
    id: root

    property string text

    Item {
        Layout.fillHeight: true
    }

    AppText {
        id: text
        text: root.text
        color: Theme.textColor
        Layout.alignment: Qt.AlignCenter
        Layout.maximumWidth: parent.width
    }

    Button {
        text: qsTr("Закрыть")
        Layout.preferredWidth: parent.width
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 16
        onClicked: root.close()
    }

    function open(text) {
        root.text = text
        root._openBase()
    }

    function close() {
        root.text = ""
        root._closeBase()
    }

    DebugBounds {}
}
