// CheckBox.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CheckBox
    \brief A checkbox with an animated state indicator.
    \inqmlmodule Revise
    \inherits Rectangle
    \since 1.0

    CheckBox provides a toggle control that can be switched between two
    states: checked and unchecked. The control is rendered as a rounded
    square filled with the theme primary color when checked, and with the
    surface container color when unchecked.

    The component provides subtle animations to give feedback to the user.
    When the pointer hovers over an unchecked box, the box slightly scales
    up. When the box is tapped, a short pop animation plays while the
    \l checked state is toggled. The transition between background colors
    is animated using an easing curve.

    The \l changed signal is emitted every time the user toggles the box,
    passing the new \l checked value as an argument. This allows the
    component to be used in forms without needing to track the state
    externally.

    \section1 Example Usage

    A basic check box bound to a model property:

    \qml
    Revise.CheckBox {
        checked: settings.notificationsEnabled
        onChanged: (value) => settings.notificationsEnabled = value
    }
    \endqml

    A checkbox with a custom size:

    \qml
    Revise.CheckBox {
        size: 24
        checked: true
    }
    \endqml

    \sa Revise.RadioButton
*/
Rectangle {
    id: root

    /*!
        \qmlproperty int CheckBox::size
        \brief The side length of the checkbox in pixels.
        \default 18

        The value is used for both \l width and \l height, keeping the
        control perfectly square. The internal animations scale the
        control around this size.
    */
    property int size: 18

    /*!
        \qmlproperty bool CheckBox::checked
        \brief Whether the checkbox is currently checked.
        \default false

        When this property is \c true, the background is filled with
        \l {Revise.Theme::primaryColor}{Revise.Theme.primaryColor} and
        the control is considered selected. When \c false, the background
        uses
        \l {Revise.Theme::surfaceContainerColor}{Revise.Theme.surfaceContainerColor}.

        Changes to this property are smoothed by the internal color
        animation.
    */
    property bool checked: false

    /*!
        \qmlsignal CheckBox::changed(bool checked)
        \brief Emitted when the user toggles the checkbox.

        The \a checked argument contains the new state of the control
        after the toggle. The signal is emitted immediately after the
        internal \l checked property has been updated.

        The corresponding handler is \c onChanged.

        \sa checked
    */
    signal changed(bool checked)

    width: root.size
    height: root.size
    radius: 4
    border.width: 1
    border.color: Revise.Theme.primaryColor

    color: root.checked
        ? Revise.Theme.primaryColor
        : Revise.Theme.surfaceContainerColor

    scale: hoverHandler.hovered && !root.checked ? 1.08 : 1.0

    Behavior on color {
        ColorAnimation {
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

    Behavior on scale {
        SpringAnimation {
            spring: 4
            damping: 0.35
            epsilon: 0.01
        }
    }

    HoverHandler {
        id: hoverHandler
        cursorShape: hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    TapHandler {
        onTapped: {
            root.checked = !root.checked
            popAnimation.restart()
            root.changed(root.checked)
        }
    }

    SequentialAnimation {
        id: popAnimation

        ScaleAnimator {
            target: root
            from: 1.0
            to: 0.85
            duration: 60
            easing.type: Easing.OutQuad
        }

        ScaleAnimator {
            target: root
            from: 0.85
            to: 1.0
            duration: 180
            easing.type: Easing.OutBack
            easing.overshoot: 2.5
        }
    }
}
