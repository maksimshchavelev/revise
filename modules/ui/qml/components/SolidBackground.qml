import QtQuick
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property alias color: backgroundOverlay.color
    property alias source: background.source

    Image {
        id: background
        anchors.fill: parent
        fillMode: Image.Tile
        visible: false
        source: "qrc:/res/img/background.svg"
    }

    ColorOverlay {
        id: backgroundOverlay
        anchors.fill: parent
        source: background
        color: Revise.Theme.backgroundLight
    }
}
