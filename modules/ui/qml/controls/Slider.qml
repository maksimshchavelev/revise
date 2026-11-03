// Slider.qml

import QtQuick
import QtQuick.Templates as T
import Revise as Revise

/*!
    \qmltype Slider
    \brief A horizontal slider for selecting a numeric value from a range.
    \inqmlmodule Revise
    \inherits T.Slider
    \since 1.0

    Slider provides a track and a draggable handle for selecting a value
    between \l from and \l to. The visual appearance is customized to match
    the Revise design system: a rounded groove and a circular handle that
    follows the \l {T.Slider::visualPosition}{visualPosition} of the control.

    The control supports horizontal orientation by default. The track and
    handle colors are derived from the Revise theme.

    \section1 Example Usage

    A basic slider:

    \qml
    Revise.Slider {
        from: 0
        to: 100
        value: 50
    }
    \endqml

    A slider with custom range and step size:

    \qml
    Revise.Slider {
        from: 1
        to: 10
        stepSize: 1
        snapMode: Slider.SnapAlways
    }
    \endqml

    \sa T.Slider
*/
T.Slider {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
        implicitHandleWidth + leftPadding + rightPadding)

    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
        implicitHandleHeight + topPadding + bottomPadding)

    padding: 8

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 6

        width: root.availableWidth
        height: implicitHeight

        x: root.leftPadding
        y: root.topPadding + (root.availableHeight - height) / 2

        radius: height / 2
        color: Color.darker(Revise.Theme.surfaceContainerColor, 1.1)

        // The track is shaded up to the slider
        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            radius: parent.radius
            color: Revise.Theme.primaryColor
        }
    }

    handle: Rectangle {
        implicitWidth: 22
        implicitHeight: 22

        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + (root.availableHeight - height) / 2

        radius: width / 2

        color: root.pressed
            ? Color.darker(Revise.Theme.primaryColor, 1.15)
            : Revise.Theme.primaryColor

        border.width: 1
        border.color: Revise.Theme.surfaceContainerColor

        Behavior on color {
            ColorAnimation { duration: 60 }
        }
    }

    HoverHandler {
        cursorShape: root.pressed ? Qt.ClosedHandCursor : Qt.PointingHandCursor
    }
}
