// The panel containing the main elements

import QtQuick
import Revise as Revise

Rectangle {
    id: root

    color: Revise.Theme.surfaceColor
    radius: 12

    TapHandler {
        onTapped: root.forceActiveFocus()
    }
}
