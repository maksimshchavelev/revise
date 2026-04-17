// Card preview page

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property bool openedAsWindow: false
    property var windowTitle: qsTr("Просмотр карточки - Revise")

    property string front : cardEditSession.front
    property string back : cardEditSession.back
    property bool flipped: false

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8
        visible: !htmlCard.loading

        Revise.CardView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: root.flipped ? root.back : root.front
        }

        Revise.Button {
            Layout.fillWidth: true
            text: qsTr("Перевернуть")
            onClicked: root.flip()
        }

        Revise.Button {
            Layout.fillWidth: true
            text: qsTr("Назад")
            onClicked: router.back()
        }
    }

    Revise.LoadingScreen {
        id: loadingScreen
        anchors.fill: parent
        thresholdTime: 0
    }

    function flip() {
        root.flipped = !root.flipped
    }

    function onEnter() {}
    function onExit() {}

    Component.onCompleted: {
        loadingScreen.startLoading()
        loadingTimer.start()
    }

    Timer {
        id: loadingTimer
        interval: 50
        onTriggered: loadingScreen.endLoading()
    }
}
