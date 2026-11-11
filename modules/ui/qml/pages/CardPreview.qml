// CardPreview.qml
// Page for Card Preview

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property bool openedAsWindow: false
    property var windowTitle: qsTr("Просмотр карточки - Revise")

    property string front
    property string back
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

        Revise.IconText {
            Layout.fillWidth: true

            icon.source: "qrc:/res/img/circle.svg"
            icon.color: Revise.Theme.primaryColor
            label.text: root.flipped ? qsTr("Обратная сторона") : qsTr("Лицевая сторона")
            label.font.bold: true
        }

        Revise.CardView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: root.flipped ? root.back : root.front
        }

        Revise.Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            text: qsTr("Перевернуть")
            onClicked: root.flip()
        }

        Revise.Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            text: qsTr("Назад")
            onClicked: Revise.Router.back()
        }
    }

    function flip() {
        root.flipped = !root.flipped
    }

    function onEnter() {
        root.flipped = false
        root.front = Revise.CardEditSession.front
        root.back = Revise.CardEditSession.back

        if (Revise.BuildInfo.isMobileBuild)
            htmlCard.active = true
    }

    function onExit() {
        // Note: On Android, WebView may remain visible even after setting
        // `visible = false`, so you must explicitly destroy it
        if (Revise.BuildInfo.isMobileBuild)
            htmlCard.active = false
    }

    Shortcut {
        sequence: "Back"
        enabled: Revise.BuildInfo.isMobileBuild
        onActivated: Revise.Router.back()
    }
}
