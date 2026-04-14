// A deck that will be displayed on the main page
import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property bool pressed: tapHandler.pressed
    property bool hovered: hoverHandler.hovered

    property string front
    property string back
    property date nextReview
    property double difficulty
    property int cardId: 0
    property int status: 0

    signal previewClicked()
    signal increaseDifficultyClicked()
    signal decreaseDifficultyClicked()
    signal editClicked()
    signal removeClicked()

    implicitHeight: layout.implicitHeight + 20

    Item {
        id: content

        anchors.fill: parent

        Rectangle {
            id: tint
            anchors.fill: parent
            color: Revise.Theme.backgroundLight
            radius: 10
        }

        ColumnLayout {
            id: layout

            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            RowLayout {
                spacing: 10

                Revise.Text {
                    text: root.front
                    font.bold: true
                    font.pixelSize: Revise.Theme.fontSizeMedium
                    maximumLineCount: 1
                    elide: Revise.Text.ElideRight
                    Layout.fillWidth: true
                }

                Revise.IconButton {
                    id: menuButton
                    size: 16
                    source: "qrc:/res/img/menu.svg"
                    color: Revise.Theme.textColorDark
                    onClicked: {
                        kekabMenu.x = menuButton.x - kekabMenu.implicitWidth - 20
                        kekabMenu.y = menuButton.y

                        kekabMenu.clampPosition()
                        kekabMenu.open()
                    }
                }
            }

            ColumnLayout {
                spacing: 3
                Layout.fillWidth: true

                property int optionsWidth: 140

                Revise.KeyValue {
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    keyWidth: parent.optionsWidth

                    key.text: qsTr("Статус:")

                    value.text: {
                        if (root.status === 0)
                            return qsTr("новая")
                        if (root.status === 1)
                            return qsTr("повторяемая")
                        if (root.status === 2)
                            return qsTr("ошибочная")
                    }

                    value.color: {
                        if (root.status === 0)
                            return Revise.Theme.green
                        if (root.status === 1)
                            return Revise.Theme.blue
                        if (root.status === 2)
                            return Revise.Theme.red
                    }
                }

                Revise.KeyValue {
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    keyWidth: parent.optionsWidth

                    key.text: qsTr("Следующий повтор:")

                    value.text: Qt.formatDateTime(root.nextReview, "dd.MM.yyyy")
                    value.color: {
                        let date = new Date()

                        if (root.nextReview.getTime() <= date.getTime()) {
                            return Revise.Theme.blue
                        }

                        return Revise.Theme.textColorDark
                    }
                }

                Revise.KeyValue {
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    keyWidth: parent.optionsWidth

                    key.text: qsTr("Сложность:")

                    value.text: root.difficulty.toFixed(1)
                    value.color: Revise.ColorUtils.colorForRange(
                                     root.difficulty, 0, 5, Revise.Theme.green,
                                     Revise.Theme.red)
                }
            }
        }
    }

    Revise.KekabMenu {
        id: kekabMenu

        Revise.KekabMenuButton {
            text: qsTr("Превью")
            onClicked: {
                kekabMenu.close()
                root.previewClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Повысить сложность")
            clickable: difficulty < 5
            onClicked: {
                kekabMenu.close()
                root.increaseDifficultyClicked()
            }
        }

        Revise.KekabMenuButton {
            text: qsTr("Понизить сложность")
            clickable: difficulty > 0
            onClicked: {
                kekabMenu.close()
                root.decreaseDifficultyClicked()
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
