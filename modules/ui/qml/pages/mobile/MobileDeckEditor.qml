// DesktopDeckEditor.qml

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import Revise as Revise


Item {
    id: root

    /*!
        \qmlproperty var DesktopDeckEditor::pageParams
        \brief The parameters passed to the page during navigation.

        Expected to contain the \c deckId key.
    */
    property var pageParams: null

    /*!
        \qmlproperty bool DesktopDeckEditor::openedAsWindow
        \brief Whether the page is opened as a separate window.

        The default value is \c false.
    */
    property bool openedAsWindow: false

    /*!
        \qmlproperty string DesktopDeckEditor::windowTitle
        \brief The title displayed in the window header.
    */
    property string windowTitle: qsTr("Редактирование колоды - Revise")

    /*!
        \qmlproperty int DesktopDeckEditor::deckId
        \brief The unique identifier of the deck being edited.

        This value is extracted from \l pageParams or defaults to \c 0.
    */
    property int deckId: pageParams ? pageParams.deckId : 0

    /*!
        \qmlproperty Revise.deck DesktopDeckEditor::deck
        \brief The actual deck object containing the data for the current deck.
    */
    property
    Revise.deck
    deck

    Revise.Background {
        anchors.fill: parent
    }

    Column {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        TabBar {
            id: tabBar

            width: parent.width
            spacing: 4
            clip: true

            background: Item {}

            // Cards
            Revise.TabButton {
                width: implicitWidth
                text: qsTr("Основное")
            }

            // Deck properties
            Revise.TabButton {
                width: implicitWidth
                text: qsTr("Параметры")
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

            // Tab with cards and search field
            ColumnLayout {
                spacing: 12

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
                        Layout.maximumWidth: implicitWidth
                        Layout.minimumWidth: 40

                        wrapMode: Revise.Text.Wrap
                        maximumLineCount: 2

                        text: {
                            if (searchField.text.trim().length === 0)
                                return qsTr(`Всего ${Revise.CardsModel.cardsCount}`)
                            return qsTr(`Найдено ${Revise.CardSearchModel.foundCardsCount}`)
                        }
                    }

                    // Search field
                    Revise.SearchField {
                        id: searchField
                        Layout.fillWidth: true
                        Layout.minimumWidth: 120
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

                            onResetClicked: dialogs.requestRelearnOneCardDialog(parent.id);

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

                            onRemoveClicked: dialogs.requestRemoveCardDialog(parent.id)
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

                onRestartLearningClicked: dialogs.requestRelearnManyCardsDialog()
                onRemoveCardsClicked: dialogs.requestRemoveManyCardsDialog()
                onRemoveDeckClicked: dialogs.requestRemoveDeckDialog(root.deckId)
                onSaveClicked: root.save()

                Connections {
                    target: deckPropertiesTab.limitTime

                    function onToggled(checked) {
                        deckPropertiesTab.limitTime.checked = checked;
                    }
                }
            }

            // Tab with card actions
            Revise.CardActionsTab {
                id: cardActionsTab

                onSetDifficultyClicked: function(value) {
                    dialogs.requestSetDifficultyForManyCardsDialog(value);
                }

                onSetStateClicked: dialogs.requestSetStateForManyCardsDialog(cardActionsTab.cardStateComboBox.displayText)
            }

            // Tab with deck statistics
            Revise.DeckStatisticsTab {}
        }
    }

    /*!
        \qmlmethod void DesktopDeckEditor::save()
        \brief Saves the current deck properties to the backend.

        This method builds a new draft using \l buildDeckDraft() and sends it
        to the \l Revise.DeckService.
    */
    function save() {
        let draft = buildDeckDraft();
        root.deck.name = draft.name;
        root.deck.description = draft.description;
        Revise.DeckService.update_deck(root.deck.id, draft)
    }

    /*!
        \qmlmethod Revise.deckDraft DesktopDeckEditor::buildDeckDraft()
        \brief Creates a draft object from the current UI values.

        \return A \l Revise.deckDraft object containing the updated deck properties.
    */
    function buildDeckDraft() {
        root.deck.timeLimit = deckPropertiesTab.limitTime.checked ? deckPropertiesTab.deckTimeLimit.value : 0

        return new Revise.deckDraft(
            deckPropertiesTab.deckName.textField.text,
            deckPropertiesTab.deckDescription.textField.text,
            root.deck.timeLimit,
            deckPropertiesTab.deckNewLimit.value,
            deckPropertiesTab.deckReviewLimit.value,
            deckPropertiesTab.deckIncorrectLimit.value)
    }

    function onEnter() {
        if (Revise.CardsModel.deckId === root.deckId)
            return;

        root.deck = Revise.DeckService.deck(root.deckId)
        deckPropertiesTab.limitTime.checked = root.deck ? root.deck.timeLimit > 0 : false
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

    // Editor dialogs
    Revise.DeckEditorDialogs {
        id: dialogs
    }

    Shortcut {
        sequence: "Back"
        enabled: Revise.BuildInfo.isMobileBuild
        onActivated: Revise.Router.back()
    }
}
