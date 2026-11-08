// DeckDelegate.qml
// A deck that will be displayed on the main page

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    width: 170
    height: 210

    property int newCardsProperty // Cards that have not yet been studied
    property int reviewCardsProperty // Cards that have already been studied and need to be reviewed
    property int incorrectCardsProperty  // Cards that the user answered incorrectly during the last study session
    property int deckIdProperty // deck id
    property int timeLimitProperty // time limit
    property string deckNameProperty // deck name
    property string deckDescriptionProperty // deck description
    property bool repeatableTodayProperty // is a deck repeatable today?

    signal studyClicked
    signal editClicked
    signal removeClicked
    signal exportClicked

    Item {
        anchors.fill: parent

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

            anchors {
                fill: parent
                margins: 8
            }

            // Deck name
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Revise.Text {
                    id: nameText
                    height: parent.height
                    width: menuButton.x - parent.x - 2
                    text: root.deckNameProperty
                    elide: Text.ElideRight
                    color: Revise.Theme.foregroundColor
                    font.bold: true
                    wrapMode: Text.Wrap

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: root.deckNameProperty
                    }
                }

                Revise.IconButton {
                    id: menuButton

                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 2

                    icon.source: "qrc:/res/img/menu.svg"
                    size: 24
                    background.color: "transparent"
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
                    text: root.deckDescriptionProperty.length > 0 ? root.deckDescriptionProperty : qsTr(
                                                                "Без описания...")
                    elide: Text.ElideRight
                    verticalAlignment: Revise.Text.AlignVCenter
                    color: Revise.Theme.secondaryForegroundColor
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    wrapMode: Text.WordWrap
                }

                Revise.HoverableTooltip {
                    visible: root.deckDescriptionProperty.length > 0
                    anchors.fill: parent
                    text: root.deckDescriptionProperty
                }
            }

            // New cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Новые")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.green
                }

                Revise.HorizontalSpacer {}

                Revise.Text {
                    text: root.newCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.newCardsProperty > 0 ? Revise.Theme.green : Revise.Theme.secondaryForegroundColor
                }
            }

            // Review cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Закрепляемые")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.blue
                }

                Revise.HorizontalSpacer {}

                Revise.Text {
                    text: root.reviewCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.reviewCardsProperty > 0 ? Revise.Theme.blue : Revise.Theme.secondaryForegroundColor
                }
            }

            // Incorrect cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Ошибочные")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.red
                }

                Revise.HorizontalSpacer {}

                Revise.Text {
                    text: root.incorrectCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.incorrectCardsProperty > 0 ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                }
            }

            // Time limit
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                property bool timeUnlimited: root.timeLimitProperty === 0
                property int iconSize: 16

                Revise.Icon {
                    size: parent.iconSize
                    source: "qrc:/res/img/clocks.svg"
                    color: Revise.Theme.foregroundColor
                }

                Revise.HorizontalSpacer {}

                Revise.Icon {
                    visible: parent.timeUnlimited
                    size: parent.iconSize
                    source: "qrc:/res/img/infinity.svg"
                    color: Revise.Theme.secondaryForegroundColor
                }

                Revise.Text {
                    visible: !parent.timeUnlimited
                    text: qsTr(`${root.timeLimitProperty} с`)
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.foregroundColor
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
                clickable: root.repeatableTodayProperty

                onClicked: root.studyClicked()

                Revise.HoverableTooltip {
                    visible: !root.repeatableTodayProperty
                    anchors.fill: parent
                    text: qsTr("Сегодня учить нечего. Отдохните")
                }
            }

            Revise.Spacer {}
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 60
            }
        }
    }

    Revise.KebabMenu {
        id: kekabMenu

        Revise.KebabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Экспортировать")
            onClicked: {
                kekabMenu.close()
                root.exportClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Удалить")
            color: Revise.Theme.red
            onClicked: {
                kekabMenu.close()
                root.removeClicked()
            }
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
