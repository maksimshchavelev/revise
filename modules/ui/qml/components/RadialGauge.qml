// RadialGauge.qml

import QtQuick
import QtQuick.Shapes
import Revise as Revise

/*!
    \qmltype RadialGauge
    \brief A circular gauge that displays a value as a filled arc.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    RadialGauge visualizes a numeric \l value relative to a \l maximum
    as a 270-degree arc with a gap at the bottom. The filled portion
    starts at the lower-left edge and grows clockwise. The current value
    is shown as text in the center of the ring, and a required \l text
    label is rendered below the gauge.

    The arc is drawn with \l {QtQuick.Shapes::Shape}{Shape} and
    \l {QtQuick.Shapes::PathAngleArc}{PathAngleArc}, which gives smooth
    antialiased rendering at any size. The component keeps a 1:1 aspect
    ratio for the ring based on its \l width, and the label below is
    accounted for in \l implicitHeight.

    The filled arc is clamped to the range \c {[0, maximum]}. Values
    outside this range affect only the rendered arc; the \l value
    property itself keeps the assigned value.

    \section1 Example Usage

    A basic gauge:

    \qml
    Revise.RadialGauge {
        width: 140
        value: 65
        text: qsTr("CPU load")
    }
    \endqml

    A customized gauge:

    \qml
    Revise.RadialGauge {
        width: 180
        maximum: 10
        value: 7
        strokeWidth: 12
        foregroundColor: Revise.Theme.success
        text: qsTr("Score")
    }
    \endqml

    \sa Revise.Spinner
*/
Item {
    id: root

    /*!
        \qmlproperty int RadialGauge::maximum
        \brief The upper bound of the value range.
        \default 100

        The filled portion of the arc is computed as
        \c {value / maximum}, clamped to the range \c {[0, 1]}. If
        \l maximum is \c 0 or negative, the filled arc is drawn empty.
    */
    property int maximum: 100

    /*!
        \qmlproperty int RadialGauge::value
        \brief The current value displayed by the gauge.
        \default 50

        This value determines both the length of the filled arc and,
        unless \l valueText is overridden, the text shown in the center
        of the ring. Values outside the range \c {[0, maximum]} are
        clamped for rendering only.
    */
    property int value: 50

    /*!
        \qmlproperty int RadialGauge::strokeWidth
        \brief The thickness of the arc stroke in pixels.
        \default 9

        The value is used for both the background track and the filled
        foreground arc. It is also taken into account when computing
        the arc radius so that the stroke stays within the component
        bounds.
    */
    property int strokeWidth: 9

    /*!
        \qmlproperty string RadialGauge::valueText
        \brief The text shown in the center of the gauge.

        By default this is bound to \l value, so the center label
        reflects the current value automatically. Assign a string to
        display a formatted value, for example \c {"42 %"}. The text
        is rendered with a single line and elided on the right if it
        does not fit.
    */
    property string valueText: root.value

    /*!
        \qmlproperty string RadialGauge::text
        \brief The caption rendered below the gauge.
        \since 1.0
        \required

        This text describes what the gauge measures, such as
        \c {"CPU load"} or \c {"Memory"}. It is placed under the ring,
        horizontally centered, and elided on the right if it exceeds
        the component width. Every RadialGauge instance must assign a
        label.
    */
    required property string text

    /*!
        \qmlproperty color RadialGauge::backgroundColor
        \brief The color of the unfilled track behind the value arc.

        The default value is
        \l {Revise.Theme::surfaceContainerColor}{Revise.Theme.surfaceContainerColor}.
    */
    property color backgroundColor: Revise.Theme.surfaceContainerColor

    /*!
        \qmlproperty color RadialGauge::foregroundColor
        \brief The color of the filled arc that represents the current value.

        The default value is
        \l {Revise.Theme::primaryColor}{Revise.Theme.primaryColor}.
    */
    property color foregroundColor: Revise.Theme.primaryColor

    /*!
        \qmlproperty real RadialGauge::implicitWidth
        \qmlproperty real RadialGauge::implicitHeight
        \brief The implicit size of the gauge, including the caption.

        The implicit width defaults to \c 120 pixels. The implicit
        height is the height of the ring (which equals the component
        width) plus the height of the caption below it.
    */
    implicitWidth: 120
    implicitHeight: shape.height + label.implicitHeight

    Shape {
        id: shape
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.width
        antialiasing: true

        ShapePath {
            strokeWidth: root.strokeWidth
            strokeColor: root.backgroundColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: shape.width / 2
                centerY: shape.height / 2
                radiusX: (Math.min(shape.width, shape.height) - root.strokeWidth) / 2
                radiusY: (Math.min(shape.width, shape.height) - root.strokeWidth) / 2
                startAngle: 135
                sweepAngle: 270
            }
        }

        ShapePath {
            strokeWidth: root.strokeWidth
            strokeColor: root.foregroundColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: shape.width / 2
                centerY: shape.height / 2
                radiusX: (Math.min(shape.width, shape.height) - root.strokeWidth) / 2
                radiusY: (Math.min(shape.width, shape.height) - root.strokeWidth) / 2
                startAngle: 135
                sweepAngle: 270 * Math.max(0, Math.min(1, root.value / root.maximum))
            }
        }

        Revise.Text {
            font.pixelSize: Revise.Theme.fontSizeGiant
            font.bold: true
            text: root.valueText
            maximumLineCount: 1
            elide: Revise.Text.ElideRight
            width: parent.width
            horizontalAlignment: Revise.Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Revise.Text {
        id: label
        width: root.width
        anchors.top: shape.bottom
        anchors.topMargin: -12
        text: root.text
        color: Revise.Theme.secondaryForegroundColor
        horizontalAlignment: Revise.Text.AlignHCenter
        elide: Revise.Text.ElideRight
        maximumLineCount: 1
    }
}
