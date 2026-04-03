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

    QQC.BusyIndicator {
        anchors.centerIn: parent
        width: parent.width * 0.35
        height: width
        running: htmlCard.loading
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
            Layout.preferredHeight: 40
            text: qsTr("Перевернуть")
            onClicked: root.flip()
        }

        Revise.Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            text: qsTr("Назад")
            onClicked: router.back()
        }
    }

    Revise.LoadingScreen {
        id: loadingScreen
        anchors.fill: parent
        visible: root.openedAsWindow
    }

    function flip() {
        root.flipped = !root.flipped
    }

    Component.onCompleted: {
        loadingScreen.startLoading()
        loadingScreen.endLoading()
    }
}
