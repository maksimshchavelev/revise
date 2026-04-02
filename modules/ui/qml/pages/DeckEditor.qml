// Edit deck page
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property var pageParams: null // Revise.Deck expected
    property string windowTitle: `${deck.name.length > 0 ? deck.name : "Unknown deck"} - Revise`

    property Revise.Deck deck: pageParams ? pageParams.deck : Revise.Deck
    property int deckId: deck.id

    onDeckIdChanged: {
        root.deck = deckService.deck(deckId)
        cardsModel.setDeck(deckId)
    }

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.background
    }

    Revise.SolidBackground {
        id: background
        anchors.fill: parent
    }

    Revise.BlurOverlay {
        id: blurOverlay
        anchors.fill: parent
        sourceItem: background
    }

    Flickable {
        id: flickable

        anchors.fill: parent
        contentWidth: width
        contentHeight: layout.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        onContentXChanged: iterateDelegates()
        onContentYChanged: iterateDelegates()

        ColumnLayout {
            id: layout

            spacing: 8

            anchors {
                fill: parent
                leftMargin: 3
                rightMargin: 3
            }

            Revise.Text {
                text: qsTr("Название")
            }

            Revise.TextField {
                id: deckName
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Имя колоды")
                text: root.deck.name
            }

            Revise.Text {
                text: qsTr("Описание")
            }

            Revise.TextField {
                id: deckDescription
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Описание (необязательно)")
                text: root.deck.description
            }

            Revise.CheckBoxOption {
                id: limitTime
                text: qsTr("Ограничить время ответа")
                Layout.fillWidth: true
                checked: root.deck ? root.deck.timeLimit > 0 : false
            }

            Revise.Text {
                text: qsTr("Ограничение времени")
            }

            Revise.TextField {
                id: deckTimeLimit
                editable: limitTime.checked
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Ограничение времени (сек)")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 3600
                }
                text: root.deck.timeLimit
            }

            Revise.Text {
                text: qsTr("Ограничение новых карточек")
            }

            Revise.TextField {
                id: deckNewLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Максимум новых карточек")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.newLimit
            }

            Revise.Text {
                text: qsTr("Ограничение повторяемых")
            }

            Revise.TextField {
                id: deckConsolidateLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Максимум повторяемых")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.consolidateLimit
            }

            Revise.Text {
                text: qsTr("Ограничение ошибочных")
            }

            Revise.TextField {
                id: deckIncorrectLimit
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                placeholder.text: qsTr("Максимум ошибочных")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 1
                    top: 500
                }
                text: root.deck.incorrectLimit
            }

            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    Layout.fillWidth: true
                    text: qsTr("Список карточек (всего " + cardsModel.cardsCount + "):")
                }

                Item {
                    Layout.fillWidth: true
                }

                // Create button
                Revise.Button {
                    Layout.preferredHeight: 40
                    text: qsTr("Добавить")
                    onClicked: {
                        let card = new Revise.Card()
                        card.deck_id = deckId
                        card.difficulty = 2.5
                        card.next_review = new Date()
                        router.navigate("cardEditor", {
                                            "editMode": false,
                                            "card": card
                                        }, Revise.page.Window)
                    }
                }
            }

            // List of the cards
            ListView {
                id: listView
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

                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width - 30

                        cardId: model.id
                        front: model.front
                        back: model.back
                        nextReview: model.nextReview
                        difficulty: model.difficulty
                        backgroundItem: blurOverlay.blurredItem

                        onPreviewClicked: {
                            let card = deckService.card(cardId)

                            cardEditSession.front = card.front
                            cardEditSession.back = card.back

                            router.navigate("cardPreview", {}, Revise.page.Window)
                        }

                        onEditClicked: {
                            router.navigate("cardEditor", {
                                                "card": deckService.card(cardId),
                                                "editMode": true
                                            }, Revise.page.Window)
                        }

                        onIncreaseDifficultyClicked: {
                            let card = deckService.card(cardId)
                            card.difficulty += 0.5
                            deckService.update_card(card)
                        }

                        onDecreaseDifficultyClicked: {
                            let card = deckService.card(cardId)
                            card.difficulty -= 0.5
                            deckService.update_card(card)
                        }

                        onRemoveClicked: {
                            deckService.remove_card(cardId)
                        }
                    }

                    function remapPosition() {
                        card.remapPosition()
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    interactive: true
                }

                onContentXChanged: iterateDelegates()
                onContentYChanged: iterateDelegates()
            }

            Revise.AcceptButton {
                Layout.alignment: Qt.AlignRight
                Layout.preferredHeight: 40
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
                    root.deck.name = deckName.text
                    root.deck.description = deckDescription.text
                    root.deck.newLimit = parseInt(deckNewLimit.text)
                    root.deck.consolidateLimit = parseInt(
                                deckConsolidateLimit.text)
                    root.deck.incorrectLimit = parseInt(deckIncorrectLimit.text)
                    root.deck.timeLimit = limitTime.checked ? parseInt(
                                                                  deckTimeLimit.text) : 0

                    deckService.update_deck(root.deck)
                }
            }

            Revise.VerticalSpacer {}
        }
    }

    Revise.CardEventDialog {
        id: cardEventDialog
        backgroundItem: flickable

        onPreviewClicked: function (cardId) {
            Qt.inputMethod.hide()
            router.navigate("cardPreview", {
                                "card": deckService.card(cardId)
                            })
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

    function iterateDelegates() {
        for (var i = 0; i < listView.contentItem.children.length; i++) {
            var child = listView.contentItem.children[i]
            if (child && typeof child.remapPosition === "function") {
                child.remapPosition()
            }
        }
    }
}
