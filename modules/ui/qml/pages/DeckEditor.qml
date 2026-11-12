// Deck editor page

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import Revise as Revise

Item {
    id: root

    property var pageParams: null // Revise.deck expected
    property bool openedAsWindow: false
    property string windowTitle: qsTr("Редактирование колоды - Revise")

    property int deckId: pageParams ? pageParams.deckId : 0

    property
    Revise.deck
    deck

    Revise.Background {
        anchors.fill: parent
    }

    SplitView {
        anchors.fill: parent
        anchors.margins: 16
        orientation: Qt.Horizontal
        handle: Revise.SplitViewHandle
        {
        }

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
                        onClicked: Revise.Router.navigate("home", {}, Revise.page.Page)
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
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth + 8
                    Layout.minimumWidth: 70

                    wrapMode: Revise.Text.Wrap
                    maximumLineCount: 2

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
                    Layout.minimumWidth: 150
                    placeholder.text: qsTr("Поиск карточек")
                    onDisplayTextChanged: searchDebounce.restart()

                    Timer {
                        id: searchDebounce
                        interval: 300
                        repeat: false
                        onTriggered: {
                            if (searchField.displayText.trim().length > 0) {
                                Revise.CardSearchModel.searchFront = searchField.displayText
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
                        let card = new Revise.card()
                        card.deckId = root.deck.id
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
                reuseItems: true
                cacheBuffer: listView.height * 2

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

                        onResetClicked: {
                            relearnCardDialog.cardId = parent.id
                            relearnCardDialog.requestOpen()
                        }

                        onRepeatTodayClicked: {
                            let draft = new Revise.updateCardDraft();
                            draft.nextReview = new Date();
                            Revise.DeckService.update_cards([parent.id], draft)
                        }

                        onIncreaseDifficultyClicked: {
                            let draft = new Revise.updateCardDraft(cardId);
                            draft.difficulty = Math.max(0.0, Math.min(card.difficulty + 0.5, 5.0));
                            Revise.DeckService.update_cards([parent.id], draft)
                        }

                        onDecreaseDifficultyClicked: {
                            let draft = new Revise.updateCardDraft(cardId);
                            draft.difficulty = Math.max(0.0, Math.max(0.0, Math.min(card.difficulty - 0.5, 5.0)));
                            Revise.DeckService.update_cards([parent.id], draft)
                        }

                        onRemoveClicked: {
                            removeCardDialog.cardId = parent.id
                            removeCardDialog.requestOpen()
                        }
                    }
                }

                ScrollBar.vertical: Revise.Scrollbar
                {
                }

                Revise.LoadingScreen {
                    id: cardsLoadingScreen
                    anchors.fill: parent
                    thresholdTime: 15
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

                background: Item {}

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
                        contentHeight: deckProperties.implicitHeight

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
                                    control: Revise.Switch {
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

                                    control: Revise.SpinBox {
                                        id: deckTimeLimit

                                        from: 0
                                        to: 3600
                                        editable: limitTime.checked
                                        value: root.deck.timeLimit

                                        Revise.HoverableTooltip {
                                            anchors.fill: parent
                                            text: qsTr("От 0 до 3600")
                                        }
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение новых карточек")

                                    control: Revise.SpinBox {
                                        id: deckNewLimit

                                        from: 1
                                        to: 1000
                                        value: root.deck.newLimit

                                        Revise.HoverableTooltip {
                                            anchors.fill: parent
                                            text: qsTr("От 1 до 1000")
                                        }
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение повторяемых карточек")

                                    control: Revise.SpinBox {
                                        id: deckReviewLimit

                                        from: 1
                                        to: 1000
                                        value: root.deck.reviewLimit

                                        Revise.HoverableTooltip {
                                            anchors.fill: parent
                                            text: qsTr("От 1 до 1000")
                                        }
                                    }
                                }

                                Revise.SettingsRow {
                                    width: parent.width
                                    text: qsTr("Ограничение ошибочных карточек")

                                    control: Revise.SpinBox {
                                        id: deckIncorrectLimit

                                        from: 1
                                        to: 1000
                                        value: root.deck.incorrectLimit

                                        Revise.HoverableTooltip {
                                            anchors.fill: parent
                                            text: qsTr("От 1 до 1000")
                                        }
                                    }
                                }
                            }

                            //  A panel showing deck actions
                            Revise.SettingsPanel {
                                width: parent.width
                                header.icon.source: "qrc:/res/img/action.svg"
                                header.label.text: qsTr("Действия с колодой")

                                // Restart learning
                                Revise.Button {
                                    width: parent.width
                                    clickable: Revise.CardsModel.cardsCount > 0

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

                                    onClicked: relearnCardsDialog.requestOpen()
                                }

                                // Remove cards
                                Revise.Button {
                                    width: parent.width
                                    color: Revise.Theme.red
                                    clickable: Revise.CardsModel.cardsCount > 0

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

                                    onClicked: removeCardsDialog.requestOpen()
                                }

                                Revise.Button {
                                    width: parent.width
                                    text: qsTr("Удалить колоду")
                                    color: Revise.Theme.red

                                    Revise.HoverableTooltip {
                                        anchors.fill: parent
                                        text: qsTr("Колода будет удалена. Действие необратимо")
                                    }

                                    onClicked: removeDeckDialog.requestOpen()
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

                            // Select all cards or deselect selected
                            Revise.Button {
                                width: parent.width
                                clickable: Revise.CardsModel.cardsCount > 0

                                text: {
                                    if (Revise.CardsModel.selectedCount === 0) {
                                        if (Revise.CardSearchModel.searchFront.length === 0)
                                            return qsTr("Выбрать всё")
                                        return qsTr("Выбрать найденное")
                                    }
                                    return qsTr("Снять выделение")
                                }

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0) {
                                            if (Revise.CardSearchModel.searchFront.length === 0)
                                                return qsTr("Выбрать все карточки в колоде")
                                            return qsTr("Выбрать только найденные карточки")
                                        }
                                        return qsTr("Снять выделение со всех карточек в колоде")
                                    }
                                }

                                onClicked: {
                                    if (Revise.CardsModel.selectedCount === 0) {
                                        if (Revise.CardSearchModel.searchFront.length === 0)
                                            Revise.CardsModel.selectAll()
                                        else
                                            Revise.CardSearchModel.selectFoundCards()
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

                            Revise.ValueSlider {
                                id: setDifficultySlider

                                width: parent.width

                                label.text: value.toFixed(1)
                                slider.from: 0.0
                                slider.to: 5.0
                                slider.stepSize: 0.1
                                value: 2.5
                            }

                            Revise.Button {
                                width: parent.width
                                clickable: Revise.CardsModel.cardsCount > 0

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

                                onClicked: {
                                    setDifficultyForCardsDialog.difficulty = setDifficultySlider.value;
                                    setDifficultyForCardsDialog.requestOpen()
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

                            Revise.ComboBox {
                                id: cardStateComboBox

                                model: ListModel {
                                    Component.onCompleted: {
                                        append({
                                            label: qsTr("Новая"),
                                            textColor: Revise.Theme.green
                                        })
                                        append({
                                            label: qsTr("Повторяемая"),
                                            textColor: Revise.Theme.blue
                                        })
                                        append({
                                            label: qsTr("Ошибочная"),
                                            textColor: Revise.Theme.red
                                        })
                                    }
                                }
                            }

                            Revise.Button {
                                width: parent.width
                                clickable: Revise.CardsModel.cardsCount > 0

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

                                onClicked: setStateForCardsDialog.requestOpen()
                            }

                            Revise.HorizontalDivider {
                                width: parent.width
                            }


                            Revise.Text {
                                width: parent.width
                                text: qsTr("Повторить карточки сегодня")
                                font.bold: true
                            }

                            // Review today
                            Revise.Button {
                                width: parent.width
                                clickable: Revise.CardsModel.cardsCount > 0

                                text: {
                                    if (Revise.CardsModel.selectedCount === 0)
                                        return qsTr("Повторить сегодня")
                                    return qsTr(`Повторить сегодня (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                                }

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: {
                                        if (Revise.CardsModel.selectedCount === 0)
                                            return qsTr("Все карточки будут повторены сегодня без сброса сложности")
                                        qsTr("Только выбранные карточки будут повторены сегодня без сброса сложности")
                                    }
                                }

                                onClicked: {
                                    let draft = new Revise.updateCardDraft();
                                    draft.nextReview = new Date();

                                    if (Revise.CardsModel.selectedCount === 0) {
                                        Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
                                    } else {
                                        Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
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
                            value: Revise.CardsModel.reviewCardsCount
                            valueText: Revise.CardsModel.cardsCount > 0 ? qsTr(`${Math.trunc(value * 100 / maximum)}%`) : "0%"
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
        let draft = createDeckDraft();
        root.deck.name = draft.name;
        root.deck.description = draft.description;
        Revise.DeckService.update_deck(root.deck.id, draft)
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
        if (Revise.CardsModel.deckId === root.deckId)
            return;

        root.deck = Revise.DeckService.deck(root.deckId)
        Revise.CardSearchModel.searchFront = ""
        Revise.CardsModel.setDeck(root.deckId)
        searchField.text = ""
    }

    function onExit() {
    }

    Component.onCompleted: {
        Revise.CardsModel.loadingStarted.connect(cardsLoadingScreen.startLoading)
        Revise.CardsModel.loadingFinished.connect(cardsLoadingScreen.endLoading)
        Revise.CardSearchModel.loadingStarted.connect(cardsLoadingScreen.startLoading)
        Revise.CardSearchModel.loadingFinished.connect(cardsLoadingScreen.endLoading)
    }

    // --------------------- Dialogs ---------------------

    // Remove one card?
    Revise.ConfirmDialog {
        id: removeCardDialog

        property int cardId

        titleText: qsTr("Удалить карточку?")
        messageText: qsTr("Карточка будет удалена без возможности восстановления")
        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveOneCard

        onAccepted: {
            Revise.SettingsService.askRemoveOneCard = !dontAskAgain
            Revise.DeckService.remove_cards([cardId])
        }
    }

    // Remove many cards?
    Revise.ConfirmDialog {
        id: removeCardsDialog

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Удалить выбранные карточки?")
            return qsTr("Удалить все карточки?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Выбранные (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточки будут удалены без возможности восстановления`)
            qsTr("Все карточки будут удалены без возможности восстановления")
        }

        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveManyCards

        onAccepted: {
            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.remove_cards(Revise.CardsModel.ids());
            } else {
                Revise.DeckService.remove_cards(Revise.CardsModel.selectedIds());
            }

            Revise.SettingsService.askRemoveManyCards = !dontAskAgain
        }
    }

    // Relearn one card?
    Revise.ConfirmDialog {
        id: relearnCardDialog

        property int cardId

        titleText: qsTr("Учить карточку заново?")
        messageText: qsTr("Прогресс обучения будет сброшен для этой карточки")
        confirmButtonText: qsTr("Учить заново")
        suppressed: !Revise.SettingsService.askRelearnOneCard

        onAccepted: {
            Revise.DeckService.reset_cards([cardId]);
            Revise.SettingsService.askRelearnOneCard = !dontAskAgain
        }
    }

    // Relearn many cards?
    Revise.ConfirmDialog {
        id: relearnCardsDialog

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Учить выбранные карточки заново?")
            return qsTr("Учить все карточки заново?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет сброшен прогресс обучения`)
            qsTr("Для всех карточек будет сброшен прогресс обучения")
        }

        confirmButtonText: qsTr("Учить заново")
        suppressed: !Revise.SettingsService.askRelearnManyCards

        onAccepted: {
            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.reset_cards(Revise.CardsModel.ids());
            } else {
                Revise.DeckService.reset_cards(Revise.CardsModel.selectedIds());
            }

            Revise.SettingsService.askRelearnManyCards = !dontAskAgain
        }
    }

    // Remove deck?
    Revise.ConfirmDialog {
        id: removeDeckDialog

        titleText: qsTr("Удалить колоду?")
        messageText: qsTr("Колода будет удалена без возможности восстановления")
        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveDeck

        onAccepted: {
            Revise.DeckService.remove_deck(root.deckId)
            Revise.Router.navigate("home", {}, Revise.page.Page)
            Revise.SettingsService.askRemoveDeck = !dontAskAgain
        }
    }

    // Set difficulty for many cards?
    Revise.ConfirmDialog {
        id: setDifficultyForCardsDialog

        property real difficulty

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Задать сложность выбранным карточкам?")
            return qsTr("Задать сложность всем карточкам?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет установлена сложность ${difficulty.toFixed(1)}`)
            qsTr(`Для всех карточек будет установлена сложность ${difficulty.toFixed(1)}`)
        }

        confirmButtonText: qsTr("Задать")
        suppressed: !Revise.SettingsService.askSetDifficultyForManyCards

        onAccepted: {
            let draft = new Revise.updateCardDraft();
            draft.difficulty = difficulty;

            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
            } else {
                Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
            }

            Revise.SettingsService.askSetDifficultyForManyCards = !dontAskAgain
        }
    }

    // Set state for many cards?
    Revise.ConfirmDialog {
        id: setStateForCardsDialog

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Задать статус выбранным карточкам?")
            return qsTr("Задать статус всем карточкам?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет установлен статус «${cardStateComboBox.displayText}»`)
            qsTr(`Для всех карточек будет установлен статус «${cardStateComboBox.displayText}»`)
        }

        confirmButtonText: qsTr("Задать")
        suppressed: !Revise.SettingsService.askSetStateForManyCards

        onAccepted: {
            let draft = new Revise.updateCardDraft();

            if (cardStateComboBox.displayText === qsTr("Новая"))
                draft.state = Revise.card.State.New
            else if (cardStateComboBox.displayText === qsTr("Повторяемая"))
                draft.state = Revise.card.State.Review
            else if (cardStateComboBox.displayText === qsTr("Ошибочная"))
                draft.state = Revise.card.State.Incorrect

            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
            } else {
                Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
            }

            Revise.SettingsService.askSetStateForManyCards = !dontAskAgain
        }
    }

    Shortcut {
        sequence: "Back"
        enabled: Revise.BuildInfo.isMobileBuild
        onActivated: Revise.Router.back()
    }
}
