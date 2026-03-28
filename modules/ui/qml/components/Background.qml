import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes
import Revise as Revise

Item {
    id: root

    property int animationDuration: 5000
    property bool animation: true

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.background
    }

    Shape {
        id: background

        anchors.fill: parent
        visible: false

        ShapePath {
            strokeWidth: 0

            fillGradient: LinearGradient {
                x1: 0
                y1: 0
                x2: width
                y2: height

                GradientStop {
                    id: leftColor
                    position: 0.0
                    color: Revise.Theme.backgroundGradient1
                }
                GradientStop {
                    id: rightColor
                    position: 1.0
                    color: Revise.Theme.backgroundGradient2
                }
            }

            PathRectangle {
                x: 0
                y: 0
                width: parent.width
                height: parent.height
            }
        }
    }

    Image {
        id: mask
        fillMode: Image.Tile
        source: "qrc:/res/img/background.svg"
        anchors.fill: parent
        visible: false
        scale: 1.35
    }

    OpacityMask {
        anchors.fill: parent
        source: background
        maskSource: mask
    }

    SequentialAnimation {
        loops: Animation.Infinite
        running: root.animation

        ParallelAnimation {
            ColorAnimation {
                target: leftColor
                property: "color"
                to: Revise.Theme.backgroundGradient2
                duration: root.animationDuration
                easing.type: Easing.InOutQuad
            }

            ColorAnimation {
                target: rightColor
                property: "color"
                to: Revise.Theme.backgroundGradient1
                duration: root.animationDuration
                easing.type: Easing.InOutQuad
            }
        }

        ParallelAnimation {
            ColorAnimation {
                target: leftColor
                property: "color"
                to: Revise.Theme.backgroundGradient1
                duration: root.animationDuration
                easing.type: Easing.InOutQuad
            }

            ColorAnimation {
                target: rightColor
                property: "color"
                to: Revise.Theme.backgroundGradient2
                duration: root.animationDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
