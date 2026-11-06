// LoadingScreen.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype LoadingScreen
    \brief An overlay with a spinner and a description that fades in and out.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    LoadingScreen covers its parent with a background, a spinner, and an
    optional description, and fades itself in and out in response to
    \l startLoading() and \l endLoading().

    Short loads are hidden entirely: the screen does not appear until the
    load has lasted \l thresholdTime milliseconds. Once visible, it stays
    on screen for at least \l minVisibleTime milliseconds, so the screen
    does not flash when a load finishes shortly after it started.

    The component has no built-in knowledge of what is being loaded. The
    caller is responsible for calling \l startLoading() when a load begins
    and \l endLoading() when it finishes.

    \section1 Example Usage

    \qml
    Revise.LoadingScreen {
        anchors.fill: parent
        description: Revise.LoadingScreenService.description

        Connections {
            target: Revise.LoadingScreenService
            function onVisibleChanged() {
                if (Revise.LoadingScreenService.visible)
                    loadingScreen.startLoading()
                else
                    loadingScreen.endLoading()
            }
        }
    }
    \endqml
*/
Item {
    id: root

    /*!
        \qmlproperty int LoadingScreen::minVisibleTime
        \brief The minimum time, in milliseconds, that the screen stays
        visible after it has appeared.

        When \l endLoading() is called, the screen waits until at least
        this much time has passed since it became visible before fading
        out. This prevents the screen from flickering on loads that finish
        right after the threshold.

        The default value is \c 300.
    */
    property int minVisibleTime: 300

    /*!
        \qmlproperty int LoadingScreen::thresholdTime
        \brief The delay, in milliseconds, before the screen appears.

        If the load finishes before this time has elapsed, the screen is
        never shown. This hides fast loads entirely.

        The default value is \c 60.
    */
    property int thresholdTime: 60

    /*!
        \qmlproperty int LoadingScreen::fadeOutDuration
        \brief The duration, in milliseconds, of the fade-out animation.

        The default value is \c 200.
    */
    property int fadeOutDuration: 200

    /*!
        \qmlproperty double LoadingScreen::startTime
        \brief The timestamp, in milliseconds, at which the screen became
        visible.

        Set internally when the threshold timer fires and used by
        \l endLoading() to compute how long the screen has been visible.
        Reading it from outside is rarely needed.

        The default value is \c 0.
    */
    property double startTime: 0

    /*!
        \qmlproperty string LoadingScreen::description
        \brief The text shown at the bottom of the screen.

        The text is word-wrapped, limited to eight lines, and elided on the
        right if it does not fit. An empty string hides the label.
    */
    property string description

    /*!
        \qmlproperty Revise.Background LoadingScreen::background
        \brief The background element that fills the screen.

        This property is an alias to the internal \l Revise.Background, so
        its color and other properties can be adjusted from the outside
        without replacing the element.
    */
    property alias background: background

    visible: root.opacity > 0
    opacity: 0

    Revise.Background {
        id: background
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

    /*!
        \qmlmethod void LoadingScreen::startLoading()
        \brief Signals that a load has started.

        Starts the threshold timer. If the load is still running after
        \l thresholdTime milliseconds, the screen fades in and records
        \l startTime. Calling this method again restarts the threshold
        timer.
    */
    function startLoading() {
        thresholdTimer.restart()
    }

    /*!
        \qmlmethod void LoadingScreen::endLoading()
        \brief Signals that the load has finished.

        Stops the threshold timer and, if the screen has already appeared,
        schedules a fade-out so that the screen remains visible for at
        least \l minVisibleTime milliseconds. If the screen has not
        appeared yet, the fade-out is scheduled immediately and the screen
        never becomes visible.
    */
    function endLoading() {
        const elapsed = new Date() - root.startTime
        hideTimer.interval = Math.max(0, root.minVisibleTime - elapsed)
        hideTimer.start()
        thresholdTimer.stop()
    }
}
