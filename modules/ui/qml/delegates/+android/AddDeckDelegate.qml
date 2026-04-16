// A deck that will be displayed on the main page
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Item {
    id: root

    width: parent.width
    height: 140

    property bool loading: false // If set to true, a loading animation will be displayed
    property string header: qsTr("Добавить колоду")
    property string description: qsTr("Вы можете либо создать новую колоду, либо импортировать существующую в формате Anki или Revise")

    signal importClicked
    signal createClicked

    Rectangle {
        id: tint
        anchors.fill: parent
        color: Revise.Theme.backgroundLight
        radius: 10
    }

    ColumnLayout {
        id: content
        visible: opacity > 0
        opacity: root.loading ? 0.0 : 1.0

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }

        anchors {
            fill: parent
            margins: 12
        }

        // Header
        Revise.Text {
            Layout.fillWidth: true
            text: root.header
            elide: Text.ElideRight
            color: Revise.Theme.textColor
            font.bold: true
        }

        Revise.VerticalSpacer {}

        Row {
            Layout.fillWidth: true
            spacing: 6

            Revise.Text {
                id: txt
                width: 150
                text: root.description
                elide: Text.ElideRight
                color: Revise.Theme.textColorDark
                font.pixelSize: Revise.Theme.fontSizeSmall
                wrapMode: Text.WordWrap
            }

            Column {
                width: parent.width - txt.width - spacing
                spacing: 6

                // Create
                Revise.Button {
                    width: parent.width
                    height: 30

                    text: qsTr("Создать")
                    font.bold: true
                    background.color: hovered ? Revise.Theme.grey : "transparent"
                    background.opacity: 0.2
                    onClicked: root.createClicked()
                }

                // Import
                Revise.Button {
                    width: parent.width
                    height: 30

                    text: qsTr("Импортировать")
                    font.bold: true
                    background.color: hovered ? Revise.Theme.grey : "transparent"
                    background.opacity: 0.2
                    onClicked: root.importClicked()
                }
            }
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
}
