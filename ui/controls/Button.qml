// A regular button with text

import QtQuick
import QtQuick.Layouts
import Revise

Item {
    id: root

    property string text
    property real fontPixelSize: Theme.textSizeDefault
    property real radius: root.height * 0.2
    property bool pressed: false
    property bool textBold: false
    property color textColor: Theme.textColor
    property bool clickable: true
    signal clicked()

    implicitWidth: textElement.implicitWidth + 40
    implicitHeight: textElement.implicitHeight + 20
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

    AppText {
        id: textElement
        anchors.centerIn: parent
        anchors.margins: 5
        text: root.text
        color: root.textColor
        font.bold: root.textBold
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

    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "black"
        opacity: 0.4
        visible: !root.clickable

        MouseArea {
            anchors.fill: parent
            onClicked: {}
            onPressed: {}
            onReleased: {}
        }
    }
}
