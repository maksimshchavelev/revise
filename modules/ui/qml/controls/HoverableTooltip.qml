// HoverableTooltip.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype HoverableTooltip
    \brief An item that shows a tooltip when the pointer hovers over it.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    HoverableTooltip attaches to an area and displays a \l Revise.Tooltip near the mouse cursor after a specified delay. It automatically clamps the tooltip position so it stays within the application window and scales its cursor offset according to the global UI scale setting.

    \section1 Example Usage

    \qml
    Revise.HoverableTooltip {
        anchors.fill: parent
        text: qsTr("Hover hint")
    }
    \endqml
*/
Item {
    id: root

    /*!
        \qmlproperty string HoverableTooltip::text
        \brief The text displayed inside the tooltip.
    */
    property string text

    /*!
        \qmlproperty real HoverableTooltip::offset
        \brief The distance in pixels from the mouse cursor to the tooltip.

        The default value is \c 8, scaled by the global UI scale setting.
    */
    property real offset: 8 * Revise.SettingsService.uiScale

    /*!
        \qmlproperty int HoverableTooltip::delay
        \brief The delay in milliseconds before the tooltip is shown.

        The default value is \c 750.
    */
    property int delay: 750

    /*!
        \qmlproperty int HoverableTooltip::windowWidth
        \brief The width of the enclosing window, used for clamping the tooltip.

        The default value is the width of the window the item belongs to, or \c 0 if no window is available.
    */
    property int windowWidth: Window.window ? Window.window.width : 0

    /*!
        \qmlproperty int HoverableTooltip::windowHeight
        \brief The height of the enclosing window, used for clamping the tooltip.

        The default value is the height of the window the item belongs to, or \c 0 if no window is available.
    */
    property int windowHeight: Window.window ? Window.window.height : 0

    Revise.Tooltip {
        id: tooltip

        x: hoverHandler.mouseX + root.offset
        y: hoverHandler.mouseY + root.offset
        text: root.text
    }

    HoverHandler {
        id: hoverHandler
        enabled: !Revise.BuildInfo.isMobileBuild

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

    /*!
        \qmlmethod void HoverableTooltip::clampPosition()
        \brief Adjusts the tooltip position to keep it within the window bounds.

        This method is called automatically before the tooltip is opened.
    */
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