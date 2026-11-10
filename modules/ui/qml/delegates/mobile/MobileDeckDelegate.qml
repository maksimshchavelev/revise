// DeckDelegate.qml
// A mobile version of a deck that will be displayed on the main page

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Rectangle {
    id: root

    width: parent.width
    height: 140
    color: Revise.Theme.surfaceColor
    radius: 10
    clip: true

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

    ColumnLayout {
        id: content

        anchors {
            fill: parent
            margins: 12
        }

        // Deck name and menu
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Revise.Text {
                Layout.fillWidth: true
                text: root.deckNameProperty
                elide: Text.ElideRight
                maximumLineCount: 2
                color: Revise.Theme.foregroundColor
                verticalAlignment: Revise.Text.AlignVCenter
                font.bold: true
                wrapMode: Revise.Text.Wrap
            }

            Revise.IconButton {
                id: menuButton
                Layout.preferredWidth: 15
                Layout.preferredHeight: 15
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                size: 15
                icon.source: "qrc:/res/img/menu.svg"
                icon.color: Revise.Theme.secondaryForegroundColor
                opacity: 0.7

                onClicked: {
                    menuLoader.open()
                }
            }
        }

        // Stats and description block
        Row {
            id: statsBlock
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            spacing: 12

            // Stats
            Column {
                id: statsColumn
                width: 150
                spacing: 6

                // New cards
                Revise.KeyValue {
                    width: parent.width

                    key.text: qsTr("Новые")
                    key.color: Revise.Theme.green
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: root.newCardsProperty
                    value.color: root.newCardsProperty > 0 ? Revise.Theme.green : Revise.Theme.secondaryForegroundColor
                    value.font.pixelSize: Revise.Theme.fontSizeSmall
                }

                // Review cards
                Revise.KeyValue {
                    width: parent.width

                    key.text: qsTr("Закрепляемые")
                    key.color: Revise.Theme.blue
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: root.reviewCardsProperty
                    value.color: root.reviewCardsProperty
                                 > 0 ? Revise.Theme.blue : Revise.Theme.secondaryForegroundColor
                    value.font.pixelSize: Revise.Theme.fontSizeSmall
                }

                // Incorrect cards
                Revise.KeyValue {
                    width: parent.width

                    key.text: qsTr("Ошибочные")
                    key.color: Revise.Theme.red
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: root.incorrectCardsProperty
                    value.color: root.incorrectCardsProperty
                                 > 0 ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                    value.font.pixelSize: Revise.Theme.fontSizeSmall
                }

                // Time limit
                RowLayout {
                    width: parent.width

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
                        text: qsTr(`${root.timeLimit} с`)
                        font.pixelSize: Revise.Theme.fontSizeSmall
                        color: Revise.Theme.foregroundColor
                        verticalAlignment: Revise.Text.AlignVCenter
                    }
                }
            } // end stats

            // Deck description and study button
            ColumnLayout {
                width: statsBlock.width - statsColumn.width - statsBlock.spacing
                height: statsBlock.implicitHeight
                spacing: 2

                // Deck description
                Revise.Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: root.deckDescriptionProperty.length > 0 ? root.deckDescriptionProperty : qsTr(
                                                                "Без описания...")
                    elide: Text.ElideRight
                    color: Revise.Theme.secondaryForegroundColor
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    wrapMode: Revise.Text.Wrap
                }

                // Study button
                Revise.Button {
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30

                    text: "Учить"
                    font.bold: true
                    clickable: root.repeatableTodayProperty

                    onClicked: root.studyClicked()
                }
            } // end deck description and study button
        }
    }

    Loader {
        id: menuLoader
        active: false
        asynchronous: false
        visible: false

        sourceComponent: Component {
            Revise.KebabMenu {
                id: kebabMenu

                Revise.KebabMenuButton {
                    text: qsTr("Редактировать")
                    onClicked: {
                        kebabMenu.close()
                        root.editClicked()
                    }
                }

                Revise.KebabMenuButton {
                    text: qsTr("Экспортировать")
                    onClicked: {
                        kebabMenu.close()
                        root.exportClicked()
                    }
                }

                Revise.KebabMenuButton {
                    text: qsTr("Удалить")
                    color: Revise.Theme.red
                    onClicked: {
                        kebabMenu.close()
                        root.removeClicked()
                    }
                }
            }
        }

        onLoaded: {
            showMenu()
        }

        function open() {
            if (!menuLoader.active)
                active = true
            else if (menuLoader.item)
                showMenu()
        }

        function showMenu() {
            menuLoader.x = menuButton.x
            menuLoader.y = menuButton.y

            item.clampPosition()
            item.open()
        }
    }
}
