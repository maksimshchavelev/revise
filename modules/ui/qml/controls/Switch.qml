// Switch.qml

import QtQuick
import Revise as Revise

/**
 * \qmltype Switch
 * \brief A modern, animated toggle switch control with smooth transitions and hover support.
 *
 * This control allows users to toggle between two states: checked (on) and unchecked (off).
 * It features built-in animations for track color transitions and thumb positioning
 *
 * \b {Example Usage (Standard Property Binding):}
 * \code
 * Switch {
 *     checked: settings.darkMode
 *     onCheckedChanged: {
 *         console.log("Dark mode status changed to:", checked)
 *     }
 * }
 * \endcode
 *
 * \b {Example Usage (Signal Handling):}
 * \code
 * Switch {
 *     onToggled: (isChecked) => {
 *         audioEngine.setMuted(isChecked)
 *     }
 * }
 * \endcode
 */
Rectangle {
    id: root

    // Is a switch checked?
    property bool checked: false

    signal toggled(bool isChecked)

    width: 48
    height: width / 2
    radius: height / 2
    color: root.checked ? Color.darker(Revise.Theme.primaryColor, 1.5) : Color.lighter(Revise.Theme.surfaceColor, 1.5)

    // Handle of switch
    Rectangle {
        width: parent.height
        height: parent.height
        radius: parent.height / 2
        x: root.checked ? (root.width - width) : 0
        color: Color.lighter(Revise.Theme.primaryColor, 1.3)

        Behavior on x {
            NumberAnimation {
                duration: 130
                easing.type: Easing.InOutQuad
            }
        }
    }

    Behavior on color {
        ColorAnimation {
            duration: 130
            easing.type: Easing.InOutQuad
        }
    }

    TapHandler {
        onTapped: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
