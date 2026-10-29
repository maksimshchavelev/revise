// PanelColumn.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/**
 * \qmltype PanelColumn
 * \brief A Revise.PanelColumn container that arranges items vertically and provides automatic vertical scrolling when layout height is constrained.
 *
 * This component layout acts like a standard \c Column but wraps its content inside an intelligent \c Flickable.
 * If the panel is given a fixed size from the outside and the internal content exceeds it, vertical scrolling
 * is enabled automatically, and a custom vertical scrollbar appears.
 *
 * \b {Example Usage (Auto-sizing, no scrollbar needed):}
 * \code
 * PanelColumn {
 *     spacing: 10
 *     Text { text: "Item 1" }
 *     Text { text: "Item 2" }
 * }
 * \endcode
 *
 * \b {Example Usage (Constrained height, triggers scrollbar if content is too tall):}
 * \code
 * PanelColumn {
 *     width: 300
 *     height: 200 // Fixed height activates conditional scrolling
 *     spacing: 12
 *
 *     // ... multiple settings fields ...
 * }
 * \endcode
 */
Revise.Panel {
    id: root

    /**
     * \qmlproperty int PanelColumn::spacing
     * The vertical gap (in pixels) between elements inside the column.
     * The default value is \c 16.
     */
    property int spacing: 16

    /**
     * \qmlproperty int PanelColumn::padding
     * The structural indentation (in pixels) from the panel borders to the inner column content.
     * The default value is \c 16.
     */
    property int padding: 16

    /**
     * \qmlproperty bool PanelColumn::scrollable
     * Determines whether the panel should allow scrolling behavior when its contents overflow.
     * The default value is \c true.
     */
    property bool scrollable: true

    default property alias content: column.data

    implicitWidth: column.childrenRect.width + root.padding * 2
    implicitHeight: column.childrenRect.height + root.padding * 2

    Flickable {
        id: flickable
        anchors.fill: parent
        clip: true

        flickableDirection: Flickable.VerticalFlick
        interactive: root.scrollable && (contentHeight > height)
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: column.height + (root.padding * 2)

        Column {
            id: column
            spacing: root.spacing

            x: root.padding
            y: root.padding

            width: parent.width - (root.padding * 2)
        }

        ScrollBar.vertical: Revise.Scrollbar
        {
            id: verticalScrollBar
            visible: flickable.interactive
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }
}
