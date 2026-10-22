// A regular button with text

import QtQuick
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property real radius: 6
    property bool pressed: tapHandler.pressed && clickable
    property bool hovered: hoverHandler.hovered && clickable
    property bool clickable: true

    property alias background: background
    property alias text: txt.text
    property alias color: txt.color
    property alias font: txt.font
    property alias horizontalAlignment: txt.horizontalAlignment
    property alias verticalAlignment: txt.verticalAlignment

    signal clicked

    implicitWidth: txt.implicitWidth + 40
    implicitHeight: Math.max(35, txt.implicitHeight + 10)
    scale: {
        if (root.pressed) {
            return 0.96
        } else if (root.hovered) {
            return 0.98
        }
        return 1.0
    }

    layer.enabled: true

    layer.effect: DropShadow {
        horizontalOffset: 4
        verticalOffset: 4
        radius: 8
        samples: 16
        color: "#20000000"
    }

    Rectangle {
        id: background
        anchors.fill: parent
        radius: root.radius
        color: Revise.Theme.surfaceContainerColor
    }

    Revise.Text {
        id: txt
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
