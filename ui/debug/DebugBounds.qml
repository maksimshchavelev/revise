// Borders around elements. Just add DebugBounds {} to your element.

import QtQuick
import Revise

Rectangle {
    visible: Debug.showBounds
    color: "transparent"
    anchors.fill: parent
    border.width: 1
    border.color: Theme.debugBounds
}
