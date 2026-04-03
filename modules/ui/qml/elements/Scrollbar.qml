import QtQuick
import QtQuick.Controls
import Revise as Revise

ScrollBar {
    policy: ScrollBar.AsNeeded
    interactive: true

    background: Rectangle {
        color: Revise.Theme.scrollbarBackground
        radius: width / 2
    }

    contentItem: Rectangle {
        implicitWidth: 8
        radius: width / 2
        color: hoverHandler.hovered ? Qt.lighter(Revise.Theme.scrollbar) : Revise.Theme.scrollbar

        HoverHandler {
            id: hoverHandler
        }
    }
}
