// Deck editor page

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Revise as Revise

Item {
    id: root

    property var pageParams: null // Revise.Deck expected
    property bool openedAsWindow: false
    property string windowTitle: qsTr("Редактирование колоды - Revise")

    property int deckId: pageParams ? pageParams.deckId : 0

    property
    Revise.Deck
    deck

    Revise.Background {
        anchors.fill: parent
    }

    SplitView {
        anchors.fill: parent
        anchors.margins: 16
        orientation: Qt.Horizontal
        handle: Revise.SplitViewHandle {}

        // Main area with deck name, description, cards, search field, etc...
        ColumnLayout {
            spacing: 12
            SplitView.fillWidth: true
            SplitView.minimumWidth: 400
            SplitView.fillHeight: true

            // Panel showing the deck name and description
            Revise.PanelColumn {
                Layout.fillWidth: true

                // Deck name
                RowLayout {
                    width: parent.width
                    spacing: 16

                    Revise.IconButton {
                        Layout.alignment: Qt.AlignVCenter
                        icon.source: "qrc:/res/img/back.svg"
                        size: Revise.Theme.fontSizeBig
                        onClicked: Revise.Router.back()
                    }

                    Revise.Text {
                        text: root.deck.name
                        font.pixelSize: Revise.Theme.fontSizeBig
                        font.bold: true
                        wrapMode: Revise.Text.WordWrap
                        elide: Revise.Text.ElideRight
                        maximumLineCount: 2
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                    }
                }

                // Deck description
                Revise.Text {
                    width: parent.width
                    color: Revise.Theme.secondaryForegroundColor
                    elide: Revise.Text.ElideRight
                    wrapMode: Revise.Text.WordWrap
                    maximumLineCount: 2

                    text: {
                        if (root.deck.description.length > 0) {
                            return root.deck.description
                        }

                        return qsTr("Описание не представлено")
                    }
                }
            }

            // Cards count, search field and add button
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: {
                        if (searchField.text.trim().length === 0)
                            return qsTr(`Список карточек (всего ${Revise.CardsModel.cardsCount}):`)

                        return qsTr(`Список карточек (найдено ${Revise.CardSearchModel.foundCardsCount}):`)
                    }
                }

                // Search field
                Revise.SearchField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholder.text: qsTr("Поиск карточек")
                    onTextChanged: searchDebounce.restart()

                    Timer {
                        id: searchDebounce
                        interval: 300
                        repeat: false
                        onTriggered: {
                            if (searchField.text.trim().length > 0) {
                                Revise.CardSearchModel.searchFront = searchField.text
                                listView.model = Revise.CardSearchModel
                            } else {
                                Revise.CardSearchModel.searchFront = ""
                                listView.model = Revise.CardsModel
                            }
                        }
                    }
                }

                // Create button
                Revise.Button {
                    text: qsTr("Создать")
                    onClicked: {
                        let card = new Revise.Card()
                        card.deck_id = root.deck.id
                        card.difficulty = 2.5
                        card.next_review = new Date()
                        Revise.Router.navigate("cardEditor", {
                            "editMode": false,
                            "card": card
                        }, Revise.page.Page)
                    }
                }
            }

            // List of the cards
            ListView {
                id: listView
                model: Revise.CardsModel
                spacing: 12
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                Layout.fillWidth: true
                Layout.fillHeight: true

                delegate: Item {
                    required property int id
                    required property string front
                    required property string back
                    required property var nextReview
                    required property double difficulty
                    required property int status
                    required property bool selected

                    width: ListView.view.width
                    height: card.implicitHeight

                    Revise.CardDelegate {
                        id: card

                        anchors.fill: parent
                        anchors.rightMargin: 25
                        anchors.leftMargin: 10

                        cardId: parent.id
                        front: parent.front
                        back: parent.back
                        nextReview: parent.nextReview
                        difficulty: parent.difficulty
                        status: parent.status
                        selected: parent.selected

                        onPreviewClicked: {
                            let card = Revise.DeckService.card(parent.id)

                            Revise.CardEditSession.front = card.front
                            Revise.CardEditSession.back = card.back

                            Revise.Router.navigate("cardPreview", {}, Revise.page.Page)
                        }

                        onEditClicked: {
                            Revise.Router.navigate("cardEditor", {
                                "card": Revise.DeckService.card(parent.id),
                                "editMode": true
                            }, Revise.page.Page)
                        }

                        onIncreaseDifficultyClicked: {
                            let card = Revise.DeckService.card(parent.id)
                            let draft = new Revise.updateCardDraft(card.front, card.back, Math.max(0.0, Math.min(card.difficulty + 0.5, 5.0)))
                            Revise.DeckService.update_card(parent.id, draft)
                        }

                        onDecreaseDifficultyClicked: {
                            let card = Revise.DeckService.card(parent.id)
                            let draft = new Revise.updateCardDraft(card.front, card.back, Math.max(0.0, Math.min(card.difficulty - 0.5, 5.0)))
                            Revise.DeckService.update_card(parent.id, draft)
                        }

                        onRemoveClicked: {
                            Revise.DeckService.remove_card(parent.id)
                        }
                    }
                }

                ScrollBar.vertical: Revise.Scrollbar
                {
                }

                Revise.LoadingScreen {
                    id: cardsLoadingScreen
                    anchors.fill: parent
                }
            }
        }

        // Right-side panel with deck properties, statistics, etc.
        Column {
            SplitView.minimumWidth: 300
            SplitView.fillHeight: true
            spacing: 8

            TabBar {
                id: tabBar

                width: parent.width
                spacing: 4
                clip: true

                // Deck properties
                Revise.TabButton {
                    width: implicitWidth
                    text: qsTr("Основное")
                }

                // Cards properties
                Revise.TabButton {
                    width: implicitWidth
                    text: qsTr("Карточки")
                }

                // Deck statistics
                Revise.TabButton {
                    width: implicitWidth
                    text: qsTr("Статистика")
                }
            }

            StackLayout {
                width: parent.width
                height: parent.height - tabBar.height - parent.spacing
                currentIndex: tabBar.currentIndex

                // Tab with deck properties and actions
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8

                    Revise.Flickable {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        contentWidth: width
                        contentHeight: deckProperties.implicitHeight + 16

                        Column {
                            id: deckProperties

                            width: parent.width - 16
                            spacing: 16

                            // Panel showing deck primary options
                            Revise.SettingsPanel {
                                width: parent.width
                                header.icon.source: "qrc:/res/img/deck.svg"
                                header.label.text: qsTr("Основные параметры")

                                Revise.FormTextField {
                                    id: deckName
                                    width: parent.width
                                    label.text: qsTr("Название колоды")
                                    textField.placeholder.text: qsTr("Введите название колоды...")
                                    textField.text: root.deck.name
                                }

                                Revise.FormTextField {
                                    id: deckDescription
                                    width: parent.width
                                    label.text: qsTr("Описание колоды (необязательно)")
                                    textField.placeholder.text: qsTr("Введите описание колоды...")
                                    textField.text: root.deck.description
                                }
                            }

                            // Panel showing deck limits
                            Revise.SettingsPanel {
                                width: parent.width
                                header.icon.source: "qrc:/res/img/restrictions.svg"
                                header.label.text: qsTr("Ограничения")

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничить время ответа")
                                    Revise.Switch {
                                        id: limitTime
                                        checked: root.deck ? root.deck.timeLimit > 0 : false
                                    }
                                }

                                Revise.HorizontalDivider {
                                    width: parent.width
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Время ответа, сек")

                                    Revise.SpinBox {
                                        id: deckTimeLimit

                                        minimum: 0
                                        maximum: 3600
                                        editable: limitTime.checked
                                        placeholder: qsTr("От 0 до 3600")
                                        value: root.deck.timeLimit
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение новых карточек")

                                    Revise.SpinBox {
                                        id: deckNewLimit

                                        minimum: 1
                                        maximum: 1000
                                        placeholder: qsTr("От 1 до 1000")
                                        value: root.deck.newLimit
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение повторяемых карточек")

                                    Revise.SpinBox {
                                        id: deckReviewLimit

                                        minimum: 1
                                        maximum: 1000
                                        placeholder: qsTr("От 1 до 1000")
                                        value: root.deck.reviewLimit
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение ошибочных карточек")

                                    Revise.SpinBox {
                                        id: deckIncorrectLimit

                                        minimum: 1
                                        maximum: 1000
                                        placeholder: qsTr("От 1 до 1000")
                                        value: root.deck.incorrectLimit
                                    }
                                }
                            }

                            //  A panel showing deck actions
                            Revise.SettingsPanel {
                                width: parent.width
                                header.icon.source: "qrc:/res/img/action.svg"
                                header.label.text: qsTr("Действия с колодой")

                                Revise.Button {
                                    width: parent.width

                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Начать обучение заново")
                                        else return qsTr(`Учить заново (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                                    }

                                    Revise.HoverableTooltip {
                                        anchors.fill: parent
                                        text: {
                                            if (Revise.CardsModel.selectedCount === 0)
                                                return qsTr("Сложность и время следующего повтора будут сброшены для всех карточек")
                                            else qsTr("Сложность и время следующего повтора будут сброшены для выбранных карточек")
                                        }
                                    }
                                }

                                Revise.Button {
                                    width: parent.width
                                    color: Revise.Theme.red

                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Удалить все карточки")
                                        else return qsTr(`Удалить карточки (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                                    }

                                    Revise.HoverableTooltip {
                                        anchors.fill: parent
                                        text: {
                                            if (Revise.CardsModel.selectedCount === 0)
                                                return qsTr("Все карточки будут удалены. Действие необратимо")
                                            else qsTr("Только выбранные карточки будут удалены. Действие необратимо")
                                        }
                                    }
                                }

                                Revise.Button {
                                    width: parent.width
                                    text: qsTr("Удалить колоду")
                                    color: Revise.Theme.red

                                    Revise.HoverableTooltip {
                                        anchors.fill: parent
                                        text: qsTr("Колода будет удалена. Действие необратимо")
                                    }
                                }
                            }
                        }
                    }

                    Revise.Button {
                        Layout.fillWidth: true
                        text: qsTr("Сохранить")
                        icon.source: "qrc:/res/img/save.svg"
                        backgroundItem.color: Revise.Theme.primaryColor
                        onClicked: root.save()
                    }
                }

                // Tab with card actions
                Revise.Flickable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    contentWidth: width
                    contentHeight: cardActions.implicitHeight

                    Column {
                        id: cardActions
                        width: parent.width

                        Revise.SettingsPanel {
                            width: parent.width
                            header.label.text: qsTr("Пакетные действия")
                            header.icon.source: "qrc:/res/img/action.svg"

                            Revise.HorizontalDivider {
                                width: parent.width
                            }

                            Revise.Button {
                                width: parent.width
                                text: {
                                    if (Revise.CardsModel.selectedCount === 0)
                                        return qsTr("Выбрать всё")
                                    return qsTr("Снять выделение")
                                }

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Выбрать все карточки в колоде")
                                        return qsTr("Снять выделение со всех карточек в колоде")
                                    }
                                }

                                onClicked: {
                                    if (Revise.CardsModel.selectedCount === 0) {
                                        Revise.CardsModel.selectAll()
                                    } else {
                                        Revise.CardsModel.deselectAll()
                                    }
                                }
                            }

                            Revise.HorizontalDivider {
                                width: parent.width
                            }

                            // Set cards difficulty
                            Revise.Text {
                                width: parent.width
                                text: qsTr("Задать сложность карточек")
                                font.bold: true
                            }

                            RowLayout {
                                width: parent.width
                                spacing: 8

                                Revise.Text {
                                    text: setDifficultySlider.value.toFixed(1)
                                }

                                Revise.Slider {
                                    id: setDifficultySlider

                                    Layout.fillWidth: true
                                    from: 0
                                    to: 5
                                    value: 2.5
                                }
                            }

                            Revise.Button {
                                width: parent.width
                                text: {
                                    if (Revise.CardsModel.selectedCount === 0)
                                        return qsTr("Задать сложность")
                                    return qsTr(`Задать сложность (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                                }

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Выбранная сложность будет установлена для всех карточек")
                                        qsTr("Выбранная сложность будет установлена для выбранных карточек")
                                    }
                                }
                            }

                            Revise.HorizontalDivider {
                                width: parent.width
                            }

                            // Set cards status
                            Revise.Text {
                                width: parent.width
                                text: qsTr("Задать статус карточек")
                                font.bold: true
                            }

                            Revise.Button {
                                width: parent.width
                                text: {
                                    if (Revise.CardsModel.selectedCount === 0)
                                        return qsTr("Задать статус")
                                    return qsTr(`Задать статус (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                                }

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Выбранный статус будет установлен для всех карточек")
                                        qsTr("Выбранный статус будет установлен для выбранных карточек")
                                    }
                                }
                            }
                        }
                    }
                }

                // Tab with deck statistics
                Revise.Flickable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    contentWidth: width
                    contentHeight: deckStatistics.implicitHeight

                    Revise.PanelColumn {
                        id: deckStatistics
                        width: parent.width - 16

                        // Study progress
                        Revise.RadialGauge {
                            width: 220
                            anchors.horizontalCenter: parent.horizontalCenter
                            maximum: Revise.CardsModel.cardsCount
                            value: Revise.CardsModel.cardsCount - Revise.CardsModel.newCardsCount - Revise.CardsModel.incorrectCardsCount
                            valueText: qsTr(`${Math.trunc(value * 100 / maximum)}%`)
                            text: qsTr("Прогресс обучения")
                        }

                        Revise.HorizontalDivider {
                            width: parent.width
                        }

                        // New cards
                        Revise.RadialGauge {
                            width: 220
                            anchors.horizontalCenter: parent.horizontalCenter
                            backgroundColor: Color.darker(Revise.Theme.green, 3)
                            foregroundColor: Revise.Theme.green
                            value: Revise.CardsModel.newCardsCount
                            maximum: Revise.CardsModel.cardsCount
                            text: qsTr("Новых карточек")
                        }

                        // Review cards
                        Revise.RadialGauge {
                            width: 220
                            anchors.horizontalCenter: parent.horizontalCenter
                            backgroundColor: Color.darker(Revise.Theme.blue, 3)
                            foregroundColor: Revise.Theme.blue
                            value: Revise.CardsModel.reviewCardsCount
                            maximum: Revise.CardsModel.cardsCount
                            text: qsTr("Повторяемых карточек")
                        }

                        // Incorrect cards
                        Revise.RadialGauge {
                            width: 220
                            anchors.horizontalCenter: parent.horizontalCenter
                            backgroundColor: Color.darker(Revise.Theme.red, 3)
                            foregroundColor: Revise.Theme.red
                            value: Revise.CardsModel.incorrectCardsCount
                            maximum: Revise.CardsModel.cardsCount
                            text: qsTr("Ошибочных карточек")
                        }
                    }
                }
            }
        }
    }

    function save() {
        Revise.DeckService.update_deck(root.deck.id, createDeckDraft())
    }

    function createDeckDraft() {
        return new Revise.deckDraft(
            deckName.textField.text,
            deckDescription.textField.text,
            deckTimeLimit.value,
            deckNewLimit.value,
            deckReviewLimit.value,
            deckIncorrectLimit.value)
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: {
            root.save()
        }
    }

    function onEnter() {
        root.deck = Revise.DeckService.deck(root.deckId)
        Revise.CardSearchModel.searchFront = ""
        Revise.CardsModel.setDeck(root.deckId)
    }

    function onExit() {}

    Component.onCompleted: {
        Revise.CardsModel.loadingStarted.connect(cardsLoadingScreen.startLoading)
        Revise.CardsModel.loadingFinished.connect(cardsLoadingScreen.endLoading)
        Revise.CardSearchModel.loadingStarted.connect(cardsLoadingScreen.startLoading)
        Revise.CardSearchModel.loadingFinished.connect(cardsLoadingScreen.endLoading)
    }
}
