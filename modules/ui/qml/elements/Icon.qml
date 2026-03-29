import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    id: root

    property int size: 16
    property alias source: img.source
    property alias color: overlay.color

    width: root.size
    height: root.size

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: false
    }

    ColorOverlay {
        id: overlay
        anchors.fill: img
        source: img
    }
}
