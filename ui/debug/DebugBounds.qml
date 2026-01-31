// Borders around elements. Just add DebugBounds {} to your element.

import QtQuick
import Revise

Rectangle {
    visible: Debug.showBounds
    color: "transparent"
    anchors.fill: parent
    border.width: 2
    border.color: Theme.debugBounds
}
