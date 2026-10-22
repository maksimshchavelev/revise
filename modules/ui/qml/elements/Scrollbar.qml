import QtQuick
import QtQuick.Controls
import Revise as Revise

ScrollBar {
    policy: ScrollBar.AsNeeded
    interactive: true
    minimumSize: 0.1

    background: Rectangle {
        color: Revise.Theme.surfaceColor
        radius: width / 2
    }

    contentItem: Rectangle {
        implicitWidth: 8
        radius: width / 2
        color: hoverHandler.hovered ? Color.lighter(Revise.Theme.surfaceColor, 1.8) : Color.lighter(Revise.Theme.surfaceColor, 1.4)

        HoverHandler {
            id: hoverHandler
        }
    }
}
