import QtQuick
import QtQuick.Shapes
import Revise as Revise

Item {
    id: root

    property bool running: true
    property int size: 36
    property color frontColor: Revise.Theme.textColor
    property color backColor: Qt.lighter(Revise.Theme.background)

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
                radiusX: width / 2
                radiusY: height / 2
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
                radiusX: width / 2
                radiusY: height / 2
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