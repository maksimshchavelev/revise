// Checkbox

import QtQuick
import "qrc:/ui/theme"

Item {
    id: root

    property bool checked: false
    property bool pressed: false

    width: 48
    height: 48
    scale: root.pressed ? 0.85 : 1

    Rectangle {
        id: background
        width: root.width * 0.55
        height: root.height * 0.55
        anchors.centerIn: root
        color: Theme.checkboxBackground
        radius: 5
    }

    Image {
        scale: root.checked ? 1 : 0
        x: background.x
        width: root.width * 0.85
        height: root.height * 0.85
        source: "qrc:/res/img/checkbox_checked.png"
        fillMode: Image.PreserveAspectFit

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.OutBack
                duration: 100
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.checked = !root.checked
        onPressed: root.pressed = true
        onReleased: root.pressed = false
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 100
        }
    }
}
