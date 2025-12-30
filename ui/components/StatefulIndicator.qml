// The indicator consists of two images.
// When it's active, one image is shown; otherwise, the other.

import QtQuick

Item {
    id: root

    property string activeIcon
    property string inactiveIcon: activeIcon
    property bool active: true
    property bool showBackground: true
    property real contentMargins: 5

    width: 96
    height: 96

    // Background
    Rectangle {
        visible: root.showBackground
        anchors.fill: parent
        opacity: 0.1
        color: "white"
        radius: height * 0.15
    }

    Image {
        source: root.active ? activeIcon : inactiveIcon
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
        anchors.margins: root.contentMargins
        anchors.centerIn: parent
    }
}
