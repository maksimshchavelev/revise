// Deck creation page
import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property string windowTitle: qsTr("Создать колоду - Revise")
    property bool openedAsWindow: false

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.background
    }

    Revise.SolidBackground {
        anchors.fill: parent
    }

    Rectangle {
        id: header
        width: root.width
        height: 60
        color: Revise.Theme.backgroundLight

        RowLayout {
            anchors {
                fill: parent
                rightMargin: 8
                leftMargin: 16
            }

            Revise.IconButton {
                Layout.alignment: Qt.AlignVCenter
                visible: !root.openedAsWindow
                source: "qrc:/res/img/back.svg"
                size: 24
                color: Revise.Theme.textColorDark
                onClicked: router.back()
            }

            Revise.HorizontalSpacer {}

            Revise.AcceptButton {
                text: qsTr("Создать")
                clickable: deckName.valid && deckName.text !== ""
                           && (deckDescription.valid || deckDescription.text
                               === "") && (!limitTime.checked
                                           || (deckTimeLimit.valid && deckTimeLimit.text
                                               !== "")) && deckNewLimit.valid
                           && deckNewLimit.text !== ""
                           && deckConsolidateLimit.valid && deckConsolidateLimit.text
                           !== "" && deckIncorrectLimit.valid
                           && deckIncorrectLimit.text !== ""

                onClicked: {
                    let deck = new Revise.Deck(deckName.text,
                                               deckDescription.text,
                                               parseInt(deckTimeLimit.text),
                                               parseInt(deckNewLimit.text),
                                               parseInt(
                                                   deckConsolidateLimit.text),
                                               parseInt(
                                                   deckIncorrectLimit.text))

                    deckService.create_deck(deck)

                    deckName.text = ""
                    deckDescription.text = ""
                    deckTimeLimit.text = ""
                    deckNewLimit.text = ""
                    deckConsolidateLimit.text = ""
                    deckIncorrectLimit.text = ""
                    limitTime.checked = false
                }
            }
        }
    }

    Flickable {
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        contentHeight: layout.implicitHeight

        anchors {
            left: root.left
            right: root.right
            top: header.bottom
            bottom: root.bottom
            margins: 4
        }

        ColumnLayout {
            id: layout
            width: parent.width
            spacing: 10

            Revise.Text {
                text: qsTr("Имя колоды")
            }

            Revise.TextField {
                id: deckName
                Layout.fillWidth: true
                placeholder.text: qsTr("Имя колоды")
                // onTextChanged: {
                //     valid = !deckService.is_deck_exists(deckName.text)
                // }
            }

            Revise.Text {
                text: qsTr("Описание колоды")
            }

            Revise.TextField {
                id: deckDescription
                Layout.fillWidth: true
                placeholder.text: qsTr("Описание (необязательно)")
            }

            Revise.CheckBoxOption {
                id: limitTime
                text: qsTr("Ограничить время ответа")
                Layout.fillWidth: true
            }

            Revise.Text {
                text: qsTr("Ограничение времени")
            }

            Revise.TextField {
                id: deckTimeLimit
                editable: limitTime.checked
                Layout.fillWidth: true
                placeholder.text: qsTr("Ограничение времени (сек)")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 3600
                }
            }

            Revise.Text {
                text: qsTr("Лимит новых карточек")
            }

            Revise.TextField {
                id: deckNewLimit
                Layout.fillWidth: true
                placeholder.text: qsTr("Максимум новых карточек")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
            }

            Revise.Text {
                text: qsTr("Лимит повторяемых карточек")
            }

            Revise.TextField {
                id: deckConsolidateLimit
                Layout.fillWidth: true
                placeholder.text: qsTr("Максимум повторяемых")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
            }

            Revise.Text {
                text: qsTr("Лимит ошибочных карточек")
            }

            Revise.TextField {
                id: deckIncorrectLimit
                Layout.fillWidth: true
                placeholder.text: qsTr("Максимум ошибочных")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
            }
        }
    }
}
