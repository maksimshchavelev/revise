// A deck that will be displayed on the main page
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    width: 170
    height: 210

    property bool loading: false // If set to true, a loading animation will be displayed
    property string header: qsTr("Добавить колоду")
    property string description: qsTr("Вы можете либо создать новую колоду, либо импортировать существующую в формате Anki или Revise")

    property Item backgroundItem: null // for blurring
    property var mappedPos: root.mapToItem(root.backgroundItem, 0, 0)

    signal importClicked
    signal createClicked

    scale: hoverHandler.hovered ? 1.02 : 1.0

    Rectangle {
        id: background

        anchors.fill: parent
        radius: 10
        color: "black"
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 4
            verticalOffset: 4
            radius: 8
            samples: 32
            color: "#80000000"
        }

        ShaderEffectSource {
            id: croppedBackground
            anchors.fill: parent
            sourceItem: root.backgroundItem
            live: true
            hideSource: true
            visible: true

            sourceRect: Qt.rect(root.mappedPos.x, root.mappedPos.y, root.width,
                                root.height)
        }
    }

    Rectangle {
        id: tint
        anchors.fill: parent
        color: Revise.Theme.deckTint
        opacity: 0.2
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
            margins: 5
        }

        Revise.VerticalSpacer {}

        // Header
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Revise.Text {
                anchors.fill: parent
                text: root.header
                elide: Text.ElideRight
                color: Revise.Theme.textColor
                verticalAlignment: Revise.Text.AlignVCenter
                horizontalAlignment: Revise.Text.AlignHCenter
                maximumLineCount: 2
                font.bold: true
            }
        }

        // Description
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 4

            Revise.Text {
                anchors.fill: parent
                text: root.description
                elide: Text.ElideRight
                verticalAlignment: Revise.Text.AlignVCenter
                color: Revise.Theme.textColorDark
                font.pixelSize: Revise.Theme.fontSizeSmall
            }
        }

        // Create
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30

            text: qsTr("Создать")
            font.bold: true
            background.color: hovered ? Revise.Theme.grey : "transparent"
            background.opacity: 0.2
            onClicked: root.createClicked()
        }

        // Import
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30

            text: qsTr("Импортировать")
            font.bold: true
            background.color: hovered ? Revise.Theme.grey : "transparent"
            background.opacity: 0.2
            onClicked: root.importClicked()
        }

        Revise.Spacer {}
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

    onXChanged: remapPosition()
    onYChanged: remapPosition()

    function remapPosition() {
        mappedPos = root.mapToItem(root.backgroundItem, 0, 0)
    }

    HoverHandler {
        id: hoverHandler
    }
}
