import QtQuick
import Revise as Revise

Item {
    id: root

    property int minVisibleTime: 300
    property int fadeOutDuration: 200
    property double startTime: 0

    visible: root.opacity > 0
    opacity: 0

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.background
    }

    Revise.Spinner {
        anchors.centerIn: parent
        size: 64
    }

    Timer {
        id: hideTimer
        interval: 0
        onTriggered: opacityAnimation.start()
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
        hideTimer.stop()
        startTime = new Date()
        root.opacity = 1.0
    }

    function endLoading() {
        const elapsed = new Date() - root.startTime
        const delay = Math.max(0, root.minVisibleTime - elapsed)

        hideTimer.interval = delay
        hideTimer.start()
    }
}
