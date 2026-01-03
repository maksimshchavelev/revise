// The element is a glass rectangle. It is similar to a
// rectangle in QML, but has a glass design.

import QtQuick
import QtQuick.Effects
import QtQuick.Controls
import "../theme"

Item {
    id: root

    property real radius: 0
    property color tintColor: Theme.tintColor
    property real tintOpacity: 0.5

    width: 200
    height: 80
    clip: true

    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        layer.enabled: false

        // Tint
        Rectangle {
            anchors.fill: parent
            radius: root.radius
            color: root.tintColor
            opacity: root.tintOpacity
        }
    }

    Item {
        id: content
        anchors.fill: parent
        anchors.margins: 12
    }

    default property alias contentData: content.data
}



