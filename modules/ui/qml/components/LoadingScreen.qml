// Loading screen

import QtQuick
import Revise as Revise

Item {
    id: root

    property int minVisibleTime: 300
    property int thresholdTime: 60 // The time period during which you can cancel the loading
    property int fadeOutDuration: 200
    property double startTime: 0
    property string description

    visible: root.opacity > 0
    opacity: 0

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.backgroundColor
    }

    Revise.Spinner {
        anchors.centerIn: parent
        size: 64
    }

    Revise.Text {
        text: root.description
        maximumLineCount: 8
        wrapMode: Text.WordWrap
        elide: Text.ElideRight

        anchors {
            horizontalCenter: root.horizontalCenter
            bottom: root.bottom
            margins: 10
            bottomMargin: 20
        }
    }

    Timer {
        id: hideTimer
        interval: 0
        onTriggered: opacityAnimation.start()
    }

    Timer {
        id: thresholdTimer
        interval: root.thresholdTime
        onTriggered: {
            hideTimer.stop()
            root.startTime = new Date()
            root.opacity = 1.0
        }
    }

    NumberAnimation {
        id: opacityAnimation
        target: root
        property: "opacity"
        to: 0
        duration: root.fadeOutDuration
        easing.type: Easing.InOutQuad
    }

    function startLoading() {
        thresholdTimer.restart()
    }

    function endLoading() {
        const elapsed = new Date() - root.startTime
        hideTimer.interval = Math.max(0, root.minVisibleTime - elapsed)
        hideTimer.start()
        thresholdTimer.stop()
    }
}
