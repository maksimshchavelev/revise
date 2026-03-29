// A regular button with text

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property real radius: root.height * 0.35
    property bool pressed: tapHandler.pressed && clickable
    property bool hovered: hoverHandler.hovered && clickable
    property bool clickable: true

    property alias background: background
    property alias text: txt.text
    property alias color: txt.color
    property alias font: txt.font

    signal clicked()

    implicitWidth: txt.implicitWidth + 40
    implicitHeight: txt.implicitHeight + 10
    scale: root.pressed ? 0.96 : 1

    Rectangle {
        id: background
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        border.width: 1
        border.color: Revise.Theme.buttonBorder
    }

    Revise.Text {
        id: txt
        color: Revise.Theme.textColor
        font.pointSize: Revise.Theme.textSizeDefault
        anchors.centerIn: parent
    }

    Rectangle {
        visible: !root.clickable
        anchors.fill: parent
        radius: root.radius
        color: "black"
        opacity: 0.3
    }

    HoverHandler {
        id: hoverHandler
        cursorShape: root.hovered && root.clickable ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    TapHandler {
        id: tapHandler
        onTapped: {
            if (root.clickable) {
                root.clicked()
            }
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }
}
