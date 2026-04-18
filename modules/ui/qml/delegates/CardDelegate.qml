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

    property Item backgroundItem: null // for blurring
    property var mappedPos: root.mapToItem(root.backgroundItem, 0, 0)

    signal previewClicked
    signal increaseDifficultyClicked
    signal decreaseDifficultyClicked
    signal editClicked
    signal removeClicked

    implicitHeight: 100

    Item {
        id: content

        anchors.fill: parent
        scale: root.hovered ? 1.01 : 1.0

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

                sourceRect: Qt.rect(root.mappedPos.x, root.mappedPos.y,
                                    root.width, root.height)
            }
        }

        Rectangle {
            id: tint
            anchors.fill: parent
            color: Revise.Theme.cardTint
            opacity: 0.3
            radius: 10
        }

        Column {
            id: layout
            width: parent.width - anchors.margins * 2
            spacing: 10

            anchors {
                centerIn: parent
                margins: 8
            }

            Row {
                width: parent.width
                spacing: 10

                Revise.Text {
                    text: root.front
                    font.bold: true
                    font.pixelSize: Revise.Theme.fontSizeMedium
                    elide: Revise.Text.ElideRight
                    width: parent.width - menuButton.width - parent.spacing

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: root.front
                    }
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
                spacing: 6
                width: parent.width

                property int optionsWidth: 140

                Revise.KeyValue {
                    width: parent.width
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Статус:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.horizontalAlignment: Text.AlignLeft
                    value.font.pixelSize: Revise.Theme.fontSizeSmall

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
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Следующий повтор:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: Qt.formatDateTime(root.nextReview, "dd.MM.yyyy")
                    value.horizontalAlignment: Text.AlignLeft
                    value.font.pixelSize: Revise.Theme.fontSizeSmall

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
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Сложность:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.horizontalAlignment: Text.AlignLeft
                    value.text: root.difficulty.toFixed(1)
                    value.font.pixelSize: Revise.Theme.fontSizeSmall
                    value.color: Revise.ColorUtils.colorForRange(
                                     root.difficulty, 0, 5, Revise.Theme.green,
                                     Revise.Theme.red)
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

    onXChanged: remapPosition()
    onYChanged: remapPosition()

    function remapPosition() {
        mappedPos = root.mapToItem(root.backgroundItem, 0, 0)
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

    TapHandler {
        id: tapHandler
    }

    HoverHandler {
        id: hoverHandler
    }
}
