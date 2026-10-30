// Spinner.qml

import QtQuick
import QtQuick.Shapes
import Revise as Revise

/*!
    \qmltype Spinner
    \inqmlmodule Revise
    \brief An animated circular loading indicator.

    The Spinner component displays a circular progress indicator consisting
    of a static background arc and a rotating foreground arc.

    The animation can be controlled using the \l running property. The
    indicator's dimensions are determined by the \l size property, while
    the foreground and background colors can be customized independently
    using \l frontColor and \l backColor.

    Example usage:
    \code
    Revise.Spinner {
        size: 24
        running: true
        frontColor: Revise.Theme.primaryColor
    }
    \endcode
*/
Item {
    id: root

    /*!
        \qmlproperty bool Spinner::running

        Whether the rotation animation is running.

        Defaults to \c true.
    */
    property bool running: true

    /*!
        \qmlproperty int Spinner::size

        The width and height of the indicator in pixels.

        Defaults to \c 36.
    */
    property int size: 36

    /*!
        \qmlproperty color Spinner::frontColor

        The color of the rotating foreground arc.

        Defaults to a lighter version of \l Revise.Theme::primaryColor.
    */
    property color frontColor: Color.lighter(Revise.Theme.primaryColor, 1.5)

    /*!
        \qmlproperty color Spinner::backColor

        The color of the static background arc.

        Defaults to a lighter version of \l Revise.Theme::surfaceColor.
    */
    property color backColor: Color.lighter(Revise.Theme.surfaceColor, 1.4)

    width: root.size
    height: root.size

    Shape {
        id: shape
        anchors.fill: root

        // Back
        ShapePath {
            strokeWidth: 3
            strokeColor: root.backColor
            fillColor: "transparent"

            PathAngleArc {
                id: staticArc
                centerX: shape.width / 2
                centerY: shape.height / 2
                radiusX: root.width / 2
                radiusY: root.height / 2
                startAngle: 0
                sweepAngle: 360
            }
        }

        // Front
        ShapePath {
            strokeWidth: 3
            strokeColor: root.frontColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                id: rotatableArc
                centerX: shape.width / 2
                centerY: shape.height / 2
                radiusX: root.width / 2
                radiusY: root.height / 2
                startAngle: 0
                sweepAngle: 70
            }
        }

        NumberAnimation on rotation {
            from: 0
            to: 360
            duration: 1000
            loops: Animation.Infinite
            running: root.running
        }
    }
}
