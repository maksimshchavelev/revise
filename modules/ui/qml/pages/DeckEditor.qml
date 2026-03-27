// Edit deck page

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    signal backClicked

    property var pageParams: null

    property Revise.Deck deck: pageParams ? pageParams.deck : Revise.Deck
    property int deckId: deck.id

    onDeckIdChanged: {
        root.deck = deckService.deck(deckId)
        cardsModel.setDeck(deckId)
    }

    Flickable {
        id: flickable

        anchors.fill: parent
        contentWidth: width
        contentHeight: layout.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: layout

            spacing: 8

            anchors {
                fill: parent
                leftMargin: 3
                rightMargin: 3
            }

            Revise.AppText {
                text: qsTr("Название")
            }

            Revise.ValidatedTextField {
                id: deckName
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Имя колоды")
                text: root.deck.name
            }

            Revise.AppText {
                text: qsTr("Описание")
            }

            Revise.ValidatedTextField {
                id: deckDescription
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholderText: qsTr("Описание (необязательно)")
                text: root.deck.description
            }

            Revise.CheckupableOption {
                id: limitTime
                text: qsTr("Ограничить время ответа")
                Layout.fillWidth: true
                checked: root.deck ? root.deck.timeLimit > 0 : false
            }

            Revise.AppText {
                text: qsTr("Ограничение времени")
            }

            Revise.ValidatedTextField {
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

            Revise.AppText {
                text: qsTr("Ограничение новых карточек")
            }

            Revise.ValidatedTextField {
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

            Revise.AppText {
                text: qsTr("Ограничение повторяемых")
            }

            Revise.ValidatedTextField {
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

            Revise.AppText {
                text: qsTr("Ограничение ошибочных")
            }

            Revise.ValidatedTextField {
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

                Revise.AppText {
                    Layout.fillWidth: true
                    text: qsTr("Список карточек (всего " + cardsModel.cardsCount + "):")
                }

                Item {
                    Layout.fillWidth: true
                }

                // Create button
                Revise.Button {
                    text: qsTr("Добавить")
                    onClicked: {
                        let card = new Revise.Card()
                        card.deck_id = deckId
                        router.navigate("cardEditor", {
                                            "editMode": false,
                                            "card": card
                                        })
                    }
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

                delegate: Item {
                    width: ListView.view.width
                    height: card.implicitHeight

                    Revise.CardDelegate {
                        id: card
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter

                        cardId: model.id
                        front: model.front
                        back: model.back

                        onClicked: cardEventDialog.open(parseInt(card.cardId))
                    }
                }
            }

            Revise.Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Обновить")
                clickable: deckName.valid && deckName.text.trim()
                           !== "" && (deckDescription.valid
                                      || deckDescription.text.trim()
                                      === "") && (!limitTime.checked
                                                  || (deckTimeLimit.valid && deckTimeLimit.text.trim(
                                                          ) !== ""))
                           && deckNewLimit.valid && deckNewLimit.text.trim(
                               ) !== ""
                           && deckConsolidateLimit.valid && deckConsolidateLimit.text.trim(
                               ) !== ""
                           && deckIncorrectLimit.valid && deckIncorrectLimit.text.trim(
                               ) !== ""
                onClicked: {
                    deckService.update_deck(root.deck)
                }
            }

            Revise.Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Закрыть")
                onClicked: router.back()
            }

            Revise.VerticalSpacer {}
        }
    }

    Revise.CardEventDialog {
        id: cardEventDialog
        backgroundItem: flickable

        onPreviewClicked: function (cardId) {
            Qt.inputMethod.hide()
            router.navigate("cardPreview", {"card": deckService.card(cardId)})
        }

        onRemoveClicked: function (cardId) {
            deckService.remove_card(cardId)
        }

        onEditClicked: function (cardId) {
            Qt.inputMethod.hide()
            router.navigate("cardEditor", {
                                "editMode": true,
                                "card": deckService.card(cardId)
                            })
        }
    }
}
