// A deck that will be displayed on the main page
import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Rectangle {
    id: root

    property string front
    property string back
    property date nextReview
    property double difficulty
    property int cardId: 0
    property int status: 0

    signal previewClicked
    signal increaseDifficultyClicked
    signal decreaseDifficultyClicked
    signal editClicked
    signal removeClicked

    implicitHeight: 90
    color: Revise.Theme.backgroundLight
    radius: 10
    clip: true

    Column {
        id: layout

        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        RowLayout {
            width: parent.width
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

        Column {
            width: parent.width
            spacing: 4

            property int optionsWidth: 140

            Revise.KeyValue {
                width: parent.width
                key.font.pixelSize: Revise.Theme.fontSizeSmall
                keyWidth: parent.optionsWidth
                key.text: qsTr("Статус:")

                value.font.pixelSize: Revise.Theme.fontSizeSmall
                value.horizontalAlignment: Text.AlignLeft

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
                width: parent.width
                key.font.pixelSize: Revise.Theme.fontSizeSmall
                keyWidth: parent.optionsWidth
                key.text: qsTr("Следующий повтор:")

                value.font.pixelSize: Revise.Theme.fontSizeSmall
                value.text: Qt.formatDateTime(root.nextReview, "dd.MM.yyyy")
                value.horizontalAlignment: Text.AlignLeft
                value.color: {
                    let date = new Date()

                    if (root.nextReview.getTime() <= date.getTime()) {
                        return Revise.Theme.blue
                    }

                    return Revise.Theme.textColorDark
                }
            }

            Revise.KeyValue {
                width: parent.width
                key.font.pixelSize: Revise.Theme.fontSizeSmall
                keyWidth: parent.optionsWidth
                key.text: qsTr("Сложность:")

                value.font.pixelSize: Revise.Theme.fontSizeSmall
                value.horizontalAlignment: Text.AlignLeft
                value.text: root.difficulty.toFixed(1)
                value.color: Revise.ColorUtils.colorForRange(
                                 root.difficulty, 0, 5, Revise.Theme.green,
                                 Revise.Theme.red)
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
