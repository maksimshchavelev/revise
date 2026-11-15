// Switch.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype Switch
    \brief An animated toggle switch control.
    \inqmlmodule Revise
    \inherits Rectangle
    \since 1.0

    Switch is a two-state toggle control. It provides smooth animations for
    track color changes and thumb movement, along with hover cursor support.

    \section1 Example Usage

    \qml
    Revise.Switch {
        checked: settings.darkMode
        onToggled: (isChecked) => {
            settings.darkMode = isChecked
        }
    }
    \endqml
*/
Rectangle {
    id: root

    /*!
        \qmlproperty bool Switch::checked
        \brief Whether the switch is in the checked (on) state.

        The default value is \c false.
    */
    property bool checked: false

    /*!
        \qmlsignal Switch::toggled(bool isChecked)
        \brief Emitted when the user toggles the switch.

        The \a isChecked argument contains the new state of the switch.
    */
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
        onTapped: root.toggled(!root.checked)
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
