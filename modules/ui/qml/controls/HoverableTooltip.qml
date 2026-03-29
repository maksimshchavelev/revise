import QtQuick
import Revise as Revise

Item {
    id: root

    property string text
    property int offset: 8
    property int delay: 750 // ms
    property int windowWidth: Window.window.width
    property int windowHeight: Window.window.height

    Revise.Tooltip {
        id: tooltip

        x: hoverHandler.mouseX + root.offset
        y: hoverHandler.mouseY + root.offset
        text: root.text
    }

    HoverHandler {
        id: hoverHandler

        property int mouseX: tooltip.opened ? mouseX : point.position.x
        property int mouseY: tooltip.opened ? mouseY : point.position.y

        onHoveredChanged: {
            if (hovered && !tooltip.opened) {
                openTimer.start()
            } else {
                openTimer.stop()
                tooltip.close()
            }
        }
    }

    Timer {
        id: openTimer
        interval: root.delay
        onTriggered: {
            clampPosition()
            tooltip.open()
        }
    }

    function clampPosition() {
        let mapped = root.mapToItem(null, 0, 0)

        let tooltipX = mapped.x + tooltip.implicitWidth + tooltip.x
        let tooltipY = mapped.y + tooltip.implicitHeight + tooltip.y

        if (tooltipX > windowWidth) {
            tooltip.x -= tooltipX - windowWidth
        }

        if (tooltipY > windowHeight) {
            tooltip.y -= tooltipY - windowHeight
        }
    }
}
