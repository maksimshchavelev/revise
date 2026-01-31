// Colorful progressbar

import QtQuick
import Revise

Item {
    id: root

    property real progress: 0 // 0 - 1

    width: 200
    height: 20

    Rectangle {
        radius: root.height / 2
        anchors.fill: parent
        color: Theme.progressbarBackgroundColor
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            radius: root.height / 2
            width: Math.min(root.width * root.progress, root.width)
            height: Math.min(root.height, width + radius)
            color: progressColor(root.progress)

            Behavior on width {
                NumberAnimation {
                    duration: 150
                }
            }

            Behavior on color {
                ColorAnimation {
                    duration: 150
                }
            }
        }
    }


    function lerp(a, b, t) {
        return a + (b - a) * t
    }

    function lerpColor(c1, c2, t) {
        return Qt.rgba(
            lerp(c1.r, c2.r, t),
            lerp(c1.g, c2.g, t),
            lerp(c1.b, c2.b, t),
            lerp(c1.a, c2.a, t)
        )
    }

    function progressColor(p) {
        p = Math.max(0, Math.min(1, p))
        var t

        if (p < 0.5) {
            t = p / 0.5
            return lerpColor(
                Theme.progressbarLowColor,
                Theme.progressbarMediumColor,
                t
            )
        } else {
            t = (p - 0.5) / 0.5
            return lerpColor(
                Theme.progressbarMediumColor,
                Theme.progressbarHighColor,
                t
            )
        }
    }
}
