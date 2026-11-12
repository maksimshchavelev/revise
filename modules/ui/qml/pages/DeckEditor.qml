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
                Revise.DeckPropertiesTab {
                    id: deckPropertiesTab

                    deckName.textField.text: root.deck.name
                    deckDescription.textField.text: root.deck.description
                    limitTime.checked: root.deck ? root.deck.timeLimit > 0 : false
                    deckTimeLimit.value: root.deck.timeLimit
                    deckNewLimit.value: root.deck.newLimit
                    deckReviewLimit.value: root.deck.reviewLimit
                    deckIncorrectLimit.value: root.deck.incorrectLimit

                    onRestartLearningClicked: removeCardsDialog.requestOpen()
                    onRemoveClicked: removeDeckDialog.requestOpen()
                    onSaveClicked: root.save()
                }

                // Tab with card actions
                Revise.CardActionsTab {
                    id: cardActionsTab

                    onSetDifficultyClicked: function(value) {
                        setDifficultyForCardsDialog.difficulty = value;
                        setDifficultyForCardsDialog.requestOpen()
                    }

                    onSetStateClicked: setStateForCardsDialog.requestOpen()
                }

                // Tab with deck statistics
                Revise.DeckStatisticsTab {}
            }
        }
    }

    function save() {
        let draft = buildDeckDraft();
        root.deck.name = draft.name;
        root.deck.description = draft.description;
        Revise.DeckService.update_deck(root.deck.id, draft)
    }

    function buildDeckDraft() {
        deckPropertiesTab.deckTimeLimit.value = deckPropertiesTab.limitTime.checked ? deckPropertiesTab.deckTimeLimit.value : 0;

        return new Revise.deckDraft(
            deckPropertiesTab.deckName.textField.text,
            deckPropertiesTab.deckDescription.textField.text,
            deckPropertiesTab.deckTimeLimit.value,
            deckPropertiesTab.deckNewLimit.value,
            deckPropertiesTab.deckReviewLimit.value,
            deckPropertiesTab.deckIncorrectLimit.value)
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
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет установлен статус «${cardActionsTab.cardStateComboBox.displayText}»`)
            qsTr(`Для всех карточек будет установлен статус «${cardActionsTab.cardStateComboBox.displayText}»`)
        }

        confirmButtonText: qsTr("Задать")
        suppressed: !Revise.SettingsService.askSetStateForManyCards

        onAccepted: {
            let draft = new Revise.updateCardDraft();

            if (cardActionsTab.cardStateComboBox.displayText === qsTr("Новая"))
                draft.state = Revise.card.State.New
            else if (cardActionsTab.cardStateComboBox.displayText === qsTr("Повторяемая"))
                draft.state = Revise.card.State.Review
            else if (cardActionsTab.cardStateComboBox.displayText === qsTr("Ошибочная"))
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
