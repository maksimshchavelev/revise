// IconButton.qml

import QtQuick
import Revise as Revise

Item {
    id: root

    property bool pressed: tapHandler.pressed && root.clickable
    property bool hovered: hoverHandler.hovered && root.clickable
    property bool clickable: true
    property int size: 36
    property alias source: icon.source
    property alias color: background.color

    signal clicked()

    width: size
    height: size
    scale: root.pressed ? 0.95 : 1

    Rectangle {
        id: background
        radius: 4
        color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
        anchors.fill: parent
        // opacity: 0.4
    }

    Revise.Icon {
        id: icon
        anchors.fill: parent
        anchors.margins: 4
    }

    TapHandler {
        id: tapHandler
        onTapped: {
            if (root.clickable) {
                root.clicked()
            }
        }
    }

    HoverHandler {
        id: hoverHandler
        cursorShape: root.hovered && root.clickable ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}


