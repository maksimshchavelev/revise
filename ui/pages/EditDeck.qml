// Edit deck
import QtQuick
import QtQuick.Layouts
import Revise
import "../controls"
import "../components"
import "../dialogs"
import "../views/training"
import "../theme"

Item {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.margins: 5

    property int deckId: 0
    property var deck: ({
                            "name": "",
                            "description": "",
                            "timeLimit": 0,
                            "newLimit": 0,
                            "consolidateLimit": 0,
                            "incorrectLimit": 0,
                            "id": 0
                        })

    signal back
    signal updateClicked(int deckId, string name, string description, int timeLimit, int newLimit, int consolidateLimit, int incorrectLimit)
    signal addCardClicked(int deckId)

    function load(deckId) {
        root.deckId = deckId
        root.deck = deckService.deck_info(deckId)
        cardsModel.load(deckId)
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: layout.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: layout
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            AppText {
                text: qsTr("Название")
            }

            ValidatedTextField {
                id: deckName
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Имя колоды")
                text: root.deck.name
            }

            AppText {
                text: qsTr("Описание")
            }

            ValidatedTextField {
                id: deckDescription
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Описание (необязательно)")
                text: root.deck.description
            }

            CheckupableOption {
                id: limitTime
                text: qsTr("Ограничить время ответа")
                Layout.fillWidth: true
                checked: root.deck ? root.deck.timeLimit > 0 : false
            }

            AppText {
                text: qsTr("Ограничение времени")
            }

            ValidatedTextField {
                id: deckTimeLimit
                editable: limitTime.checked
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Ограничение времени (сек)")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 3600
                }
                text: root.deck.timeLimit
            }

            AppText {
                text: qsTr("Ограничение новых карточек")
            }

            ValidatedTextField {
                id: deckNewLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Максимум новых карточек")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.newLimit
            }

            AppText {
                text: qsTr("Ограничение повторяемых")
            }

            ValidatedTextField {
                id: deckConsolidateLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Максимум повторяемых")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.consolidateLimit
            }

            AppText {
                text: qsTr("Ограничение ошибочных")
            }

            ValidatedTextField {
                id: deckIncorrectLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Максимум ошибочных")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.incorrectLimit
            }

            RowLayout {
                Layout.fillWidth: true

                AppText {
                    text: qsTr("Список карточек:")
                }

                Item {
                    Layout.fillWidth: true
                }

                // Create button
                Button {
                    text: qsTr("Добавить")
                    onClicked: root.addCardClicked(root.deckId)
                }
            }

            // List of the cards
            ListView {
                model: cardsModel
                spacing: 10
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight,
                                                 root.height * 0.55)

                onContentHeightChanged: console.log(contentHeight)

                delegate: Item {
                    width: ListView.view.width
                    height: card.implicitHeight

                    CardView {
                        id: card
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter

                        cardId: model.id
                        front: model.front
                        back: model.back

                        onClicked: {
                            console.log("Clicked on the card with id = " + cardId)
                        }
                    }
                }
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Обновить")
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
                    updateClicked(
                                root.deckId, deckName.text,
                                deckDescription.text,
                                limitTime.checked ? parseInt(
                                                        deckTimeLimit.text) : 0,
                                parseInt(deckNewLimit.text),
                                parseInt(deckConsolidateLimit.text),
                                parseInt(deckIncorrectLimit.text))
                }
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Закрыть")
                onClicked: root.back()
            }
        }
    }
}
