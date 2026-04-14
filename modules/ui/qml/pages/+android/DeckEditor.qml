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
    property bool openedAsWindow: false
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

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 4

            Revise.Text {
                text: qsTr("Название")
            }

            Revise.TextField {
                id: deckName
                Layout.fillWidth: true
                placeholder.text: qsTr("Имя колоды")
                text: root.deck.name
            }

            Revise.Text {
                text: qsTr("Описание")
            }

            Revise.TextField {
                id: deckDescription
                Layout.fillWidth: true
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
                    text: {
                        if (searchField.text.trim().length === 0) {
                            return qsTr("Список карточек (всего " + cardsModel.cardsCount + "):")
                        }

                        return qsTr("Список карточек (найдено " + cardsModel.cardsCount + "):")
                    }
                }

                Revise.SearchField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholder.text: qsTr("Что хотите найти?")
                    onTextChanged: searchDebounce.restart()

                    Timer {
                        id: searchDebounce
                        interval: 300
                        repeat: false
                        onTriggered: cardsModel.searchFront = searchField.text
                    }
                }

                // Create button
                Revise.Button {
                    text: qsTr("Добавить")
                    onClicked: {
                        let card = new Revise.Card()
                        card.deck_id = deckId
                        card.difficulty = 2.5
                        card.next_review = new Date()
                        router.navigate("cardEditor", {
                                            "editMode": false,
                                            "card": card
                                        }, Revise.page.Page)
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
                Layout.preferredHeight: root.height * 0.55

                delegate: Item {
                    width: ListView.view.width
                    height: card.implicitHeight

                    Revise.CardDelegate {
                        id: card

                        anchors {
                            fill: parent
                            leftMargin: 10
                            rightMargin: 25
                        }

                        cardId: model.id
                        front: model.front
                        back: model.back
                        nextReview: model.nextReview
                        difficulty: model.difficulty
                        status: model.status

                        onPreviewClicked: {
                            let card = deckService.card(cardId)

                            cardEditSession.front = card.front
                            cardEditSession.back = card.back

                            router.navigate("cardPreview", {}, Revise.page.Page)
                        }

                        onEditClicked: {
                            router.navigate("cardEditor", {
                                                "card": deckService.card(cardId),
                                                "editMode": true
                                            }, Revise.page.Page)
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
                }

                ScrollBar.vertical: Revise.Scrollbar {}

                Revise.LoadingScreen {
                    id: cardsLoadingScreen
                    anchors.fill: parent
                }
            }


            RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.preferredHeight: 40
                Layout.margins: 5

                Revise.Button {
                    visible: !root.openedAsWindow
                    Layout.fillHeight: true
                    text: qsTr("Назад")
                    onClicked: router.back()
                }

                Revise.AcceptButton {
                    id: updateButton
                    text: qsTr("Обновить")
                    Layout.fillHeight: true
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
                        root.update()
                    }
                }
            }

            Revise.VerticalSpacer {}
        }
    }

    function update() {
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

    Component.onCompleted: cardsModel.searchFront = ""

    Connections {
        target: cardsModel

        function onLoadingStarted() {
            cardsLoadingScreen.startLoading()
        }

        function onLoadingFinished() {
            cardsLoadingScreen.endLoading()
        }
    }
}
