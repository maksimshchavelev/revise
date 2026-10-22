import QtQuick
import Revise as Revise

Revise.Icon {
    id: root

    property bool pressed: tapHandler.pressed && root.clickable
    property bool hovered: hoverHandler.hovered && root.clickable
    property bool clickable: true

    signal clicked()

    scale: root.pressed ? 0.95 : 1

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
