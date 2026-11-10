// AddDeckDelegate.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    width: 170
    height: 210

    property bool loading: false // If set to true, a loading animation will be displayed
    property string header: qsTr("Добавить колоду")
    property string description: qsTr("Вы можете создать новую колоду или импортировать существующую из Anki или Revise")

    signal importClicked
    signal createClicked

    scale: hoverHandler.hovered ? 1.02 : 1.0

    Rectangle {
        id: tint
        anchors.fill: parent
        color: Revise.Theme.surfaceColor
        radius: 10
        layer.effect: Revise.Shadow {}
    }

    ColumnLayout {
        id: content
        visible: opacity > 0
        opacity: root.loading ? 0.0 : 1.0
        spacing: 8

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }

        anchors {
            fill: parent
            margins: 8
        }

        // Header
        Revise.Text {
            Layout.fillWidth: true
            text: root.header
            elide: Text.ElideRight
            color: Revise.Theme.foregroundColor
            font.bold: true
        }

        Revise.Spacer {}

        // Description
        Revise.Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: root.description
            elide: Text.ElideRight
            color: Revise.Theme.secondaryForegroundColor
            font.pixelSize: Revise.Theme.fontSizeSmall
            wrapMode: Text.WordWrap
        }

        // Create
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30

            text: qsTr("Создать")
            font.bold: true
            onClicked: root.createClicked()
        }

        // Import
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30
            Layout.bottomMargin: 4

            text: qsTr("Импортировать")
            font.bold: true
            onClicked: root.importClicked()
        }
    }

    Revise.Spinner {
        id: spinner
        visible: opacity > 0
        opacity: root.loading ? 1.0 : 0.0
        running: true
        size: 36
        anchors.centerIn: parent

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
