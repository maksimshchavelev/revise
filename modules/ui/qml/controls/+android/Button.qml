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
    property alias horizontalAlignment: txt.horizontalAlignment
    property alias verticalAlignment: txt.verticalAlignment

    signal clicked()

    implicitWidth: txt.implicitWidth + 40
    implicitHeight: Math.max(40, txt.implicitHeight + 10)
    scale: root.pressed ? 0.96 : 1

    Rectangle {
        id: background
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        opacity: 0.2
        border.width: 2
        border.color: root.pressed ? "transparent" : Revise.Theme.buttonBorder
    }

    Revise.Text {
        id: txt
        color: Revise.Theme.textColor
        font.pixelSize: Revise.Theme.fontSizeDefault
        anchors.fill: parent
        anchors.margins: 8
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
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
