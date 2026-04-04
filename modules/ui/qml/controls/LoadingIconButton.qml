// A button that can be either an icon or a loading spinner

import QtQuick
import Qt5Compat.GraphicalEffects
import Revise as Revise

Rectangle {
    id: root

    property alias source: icon.source
    property alias spinner: spinner
    property int size: 48
    property bool loading: false

    property bool pressed: tapHandler.pressed && root.clickable
    property bool hovered: hoverHandler.hovered && root.clickable
    property bool clickable: true

    radius: height / 2
    width: root.size
    height: root.size
    color: Revise.Theme.loadingIconButtonBackground
    scale: root.pressed ? 0.95 : 1

    signal clicked()

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 5
        verticalOffset: 5
        radius: 8
        samples: 32
        color: "#80000000"
    }

    Revise.Icon {
        id: icon
        anchors.fill: parent
        visible: !root.loading
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.35
        visible: !root.clickable
    }

    Revise.Spinner {
        id: spinner
        anchors.fill: parent
        running: root.loading
        visible: root.loading
        backColor: Qt.lighter(Revise.Theme.loadingIconButtonBackground)
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
