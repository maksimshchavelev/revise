// A deck that will be displayed on the main page
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Item {
    id: root

    width: parent.width
    height: 140

    property int newCards: 0 // Cards that have not yet been studied
    property int consolidateCards: 0 // Cards that have already been studied and need to be reviewed
    property int incorrectCards: 0 // Cards that the user answered incorrectly during the last training session
    property int deckId: 0 // deck id
    property int timeLimit: 0 // time limit
    property string deckName
    property string deckDescription
    property bool repeatableToday: false

    signal studyClicked
    signal editClicked
    signal removeClicked
    signal exportClicked

    Item {
        anchors.fill: parent

        Rectangle {
            id: tint
            anchors.fill: parent
            color: Revise.Theme.backgroundLight
            radius: 10
        }

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
                    text: root.deckName
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    color: Revise.Theme.textColor
                    verticalAlignment: Revise.Text.AlignVCenter
                    font.bold: true
                }

                Revise.IconButton {
                    id: menuButton
                    Layout.preferredWidth: 15
                    Layout.preferredHeight: 15
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

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

            // Stats and description block
            RowLayout {
                id: statsBlock
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignBottom
                spacing: 12

                // Stats
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 150
                    Layout.maximumWidth: 150

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
                            text: root.newCards
                            font.pixelSize: Revise.Theme.fontSizeSmall
                            color: root.newCards > 0 ? Revise.Theme.green : Revise.Theme.textColorDark
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
                            text: root.consolidateCards
                            font.pixelSize: Revise.Theme.fontSizeSmall
                            color: root.consolidateCards
                                   > 0 ? Revise.Theme.blue : Revise.Theme.textColorDark
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
                            text: root.incorrectCards
                            font.pixelSize: Revise.Theme.fontSizeSmall
                            color: root.incorrectCards
                                   > 0 ? Revise.Theme.red : Revise.Theme.textColorDark
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
                            font.pixelSize: Revise.Theme.fontSizeSmall
                            color: Revise.Theme.textColor
                            verticalAlignment: Revise.Text.AlignVCenter
                        }
                    }
                } // end stats

                // Deck description and study button
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: statsBlock.implicitHeight
                    Layout.maximumHeight: statsBlock.implicitHeight
                    spacing: 2

                    // Deck description
                    Revise.Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: root.deckDescription.length > 0 ? root.deckDescription : qsTr(
                                                                    "Без описания...")
                        elide: Text.ElideRight
                        color: Revise.Theme.textColorDark
                        font.pixelSize: Revise.Theme.fontSizeSmall
                        wrapMode: Text.WordWrap
                    }

                    // Study button
                    Revise.Button {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        text: "Учить"
                        font.bold: true
                        clickable: root.repeatableToday
                        background.color: hovered ? Revise.Theme.grey : "transparent"
                        background.opacity: 0.2

                        onClicked: root.studyClicked()
                    }
                } // end deck description and study button
            }
        }
    }

    Revise.KekabMenu {
        id: kekabMenu

        Revise.KekabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Экспортировать")
            onClicked: {
                kekabMenu.close()
                root.exportClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Удалить")
            color: Revise.Theme.red
            onClicked: {
                kekabMenu.close()
                root.removeClicked()
            }
        }
    }
}
