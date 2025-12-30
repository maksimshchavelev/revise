// A standard toggle switch. Shows an active
// icon when enabled and an inactive icon when disabled.

import QtQuick
import "qrc:/ui/theme"

Item {
    id: root

    property string activeIcon
    property string inactiveIcon: activeIcon
    property real radius: root.height * 0.2
    property bool pressed: false
    property bool checked: false
    signal clicked()

    width: 96
    height: 96
    scale: root.pressed ? 0.98 : 1

    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "white"
        opacity: root.pressed ? 0.1 : 0.15

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutBack
                duration: 90
            }
        }
    }

    Image {
        anchors.fill: parent
        anchors.margins: 5
        anchors.centerIn: parent
        source: checked ? root.activeIcon : root.inactiveIcon
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.clicked()
        onPressed: root.pressed = true
        onReleased: root.pressed = false
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 90
        }
    }

    onClicked: root.checked = !root.checked
}
