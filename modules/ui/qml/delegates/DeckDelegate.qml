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
    scale: hoverHandler.hovered ? 1.02 : 1.0

    property int newCards: 0 // Cards that have not yet been studied
    property int consolidateCards: 0 // Cards that have already been studied and need to be reviewed
    property int incorrectCards: 0 // Cards that the user answered incorrectly during the last training session
    property int deckId: 0 // deck id
    property int timeLimit: 0 // time limit
    property string deckName
    property string deckDescription
    property bool repeatableToday: false

    property Item backgroundItem: null // for blurring
    property var mappedPos: root.mapToItem(root.backgroundItem, 0, 0)

    signal studyClicked()
    signal editClicked()
    signal removeClicked()

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

        anchors {
            fill: parent
            margins: 5
        }

        // Deck name
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Revise.Text {
                height: parent.height
                width: menuButton.x - parent.x
                text: root.deckName
                elide: Text.ElideRight
                color: Revise.Theme.textColor
                verticalAlignment: Revise.Text.AlignVCenter
                font.bold: true

                Revise.HoverableTooltip {
                    anchors.fill: parent
                    text: root.deckName
                }
            }

            Revise.IconButton {
                id: menuButton

                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 5

                source: "qrc:/res/img/menu.svg"
                size: 15
                color: Revise.Theme.textColorDark
                opacity: 0.7

                onClicked: {
                    kekabMenu.x = menuButton.x + menuButton.width + 10
                    kekabMenu.y = menuButton.y
                    kekabMenu.clampPosition()
                    kekabMenu.open()
                }
            }
        }

        // Deck description
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Revise.Text {
                anchors.fill: parent
                text: root.deckDescription.length > 0 ? root.deckDescription : qsTr(
                                                            "Без описания...")
                elide: Text.ElideRight
                verticalAlignment: Revise.Text.AlignVCenter
                color: Revise.Theme.textColorDark
                font.pointSize: Revise.Theme.fontSizeSmall
            }

            Revise.HoverableTooltip {
                visible: root.deckDescription.length > 0
                anchors.fill: parent
                text: root.deckDescription
            }
        }

        // New cards
        RowLayout {
            Layout.fillWidth: true

            Revise.Text {
                text: qsTr("Новые")
                font.pointSize: Revise.Theme.fontSizeSmall
                color: Revise.Theme.green
            }

            Revise.HorizontalSpacer {}

            Revise.Text {
                text: root.newCards
                font.pointSize: Revise.Theme.fontSizeSmall
                color: root.newCards > 0 ? Revise.Theme.green : Revise.Theme.textColorDark
            }
        }

        // Review cards
        RowLayout {
            Layout.fillWidth: true

            Revise.Text {
                text: qsTr("Закрепляемые")
                font.pointSize: Revise.Theme.fontSizeSmall
                color: Revise.Theme.blue
            }

            Revise.HorizontalSpacer {}

            Revise.Text {
                text: root.consolidateCards
                font.pointSize: Revise.Theme.fontSizeSmall
                color: root.newCards > 0 ? Revise.Theme.blue : Revise.Theme.textColorDark
            }
        }

        // Incorrect cards
        RowLayout {
            Layout.fillWidth: true

            Revise.Text {
                text: qsTr("Ошибочные")
                font.pointSize: Revise.Theme.fontSizeSmall
                color: Revise.Theme.red
            }

            Revise.HorizontalSpacer {}

            Revise.Text {
                text: root.consolidateCards
                font.pointSize: Revise.Theme.fontSizeSmall
                color: root.newCards > 0 ? Revise.Theme.red : Revise.Theme.textColorDark
            }
        }

        // Time limit
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            property bool timeUnlimited: root.timeLimit === 0
            property int iconSize: 16

            Revise.Icon {
                size: parent.iconSize
                source: "qrc:/res/img/clocks.svg"
                color: Revise.Theme.textColor
            }

            Revise.HorizontalSpacer {}

            Revise.Icon {
                visible: parent.timeUnlimited
                size: parent.iconSize
                source: "qrc:/res/img/infinity.svg"
                color: Revise.Theme.textColorDark
            }

            Revise.Text {
                visible: !parent.timeUnlimited
                text: qsTr(`${root.timeLimit} с`)
                font.pointSize: Revise.Theme.fontSizeSmall
                color: Revise.Theme.textColor
                verticalAlignment: Revise.Text.AlignVCenter
            }
        }

        Revise.Spacer {}

        Revise.Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width * 0.85
            Layout.preferredHeight: 30

            text: "Учить"
            font.bold: true
            clickable: root.repeatableToday
            background.color: hovered ? Revise.Theme.grey : "transparent"
            background.opacity: 0.2

            onClicked: root.studyClicked()

            Revise.HoverableTooltip {
                visible: !root.repeatableToday
                anchors.fill: parent
                text: qsTr("Сегодня учить нечего. Отдохните")
            }
        }

        Revise.Spacer {}
    }

    Revise.KekabMenu {
        id: kekabMenu

        Revise.KekabMenuButton {
            text: qsTr("Редактировать")
            font.pointSize: Revise.Theme.fontSizeSmall
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Удалить")
            color: Revise.Theme.red
            font.pointSize: Revise.Theme.fontSizeSmall
            onClicked: {
                kekabMenu.close()
                root.removeClicked()
            }
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

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }
}
