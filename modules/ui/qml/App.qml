import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Dialogs
import QtQuick.Layouts
import Revise

ApplicationWindow {
    id: appWindow
    visible: true

    minimumWidth: 480
    minimumHeight: 320

    width: 720
    height: 560

    title: "Revise"

    background: Image {
        fillMode: Image.Tile
        source: "qrc:/res/img/background_blurred.png"
    }

    readonly property var pages: {
        "main": "qrc:/qml/MainLayout.qml",
        "training": "qrc:/qml/pages/Training.qml",
        "editDeck": "qrc:/qml/pages/EditDeck.qml",
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: pages.main
        asynchronous: true

        property bool loading: status === Loader.Loading

        onLoaded: {
            if (item.addDeckClicked) {
                item.addDeckClicked.connect(addDeckPopup.open)
            }
            if (item.deckClicked) {
                item.deckClicked.connect(function(deckId, repeatableToday) {
                    deckModalDialog.open(deckId, repeatableToday)
                })
            }
        }
    }

    Connections {
        target: pageLoader.item
        ignoreUnknownSignals: true

        function onTrainingFinished() {
            goToMain()
        }

        function onAborted() {
            goToMain()
        }

        function onBackClicked() {
            goToMain()
        }
    }

    function goToMain() {
        // !!!
        pageLoader.active = false

        Qt.callLater(() => {
            pageLoader.source = pages.main
            pageLoader.active = true
        })
    }

    CreateDeckPopup {
        id: createDeckPopup
        backgroundItem: pageLoader

        onCreateClicked: function(deck) {
            deckService.create_deck(deck)
        }
    }

    InfoPopup {
        id: infoPopup
        backgroundItem: pageLoader
    }

    ScrollableInfoPopup {
        id: scrollableInfoPopup
        sourceItem: pageLoader
    }

    FileDialog {
        id: importAnkiFileDialog
        title: qsTr("Выберите файл")
        nameFilters: [
            qsTr("Колода Revise (*.rpkg)"),
            qsTr("Колода Anki (*.apkg")
        ]
        onAccepted: deckService.import_deck_async(selectedFile)
    }

    FolderDialog {
        property int deckId: 0

        id: exportAnkiFolderDialog
        title: qsTr("Сохранить .rpkg")
        onAccepted: {
            deckService.export_deck_async(deckId, selectedFolder)
        }
    }

    AddDeckPopup {
        id: addDeckPopup
        backgroundItem: pageLoader

        onImportClicked: importAnkiFileDialog.open()
        onCreateClicked: createDeckPopup.open()
    }

    DeckModalDialog {
        id: deckModalDialog
        sourceItem: pageLoader

        onTrainClicked: function(deckId, repeatableToday) {
            if (repeatableToday) {
                studyService.start_training(deckId)
                pageLoader.source = pages.training
            } else {
                infoPopup.open(qsTr("Колоду нельзя повторить сегодня"))
            }
        }

        onRemoveClicked: function(deckId) {
            deckRemoveQuestionPopup.deckId = deckId
            deckRemoveQuestionPopup.open(qsTr("Удалить колоду?"))
        }

        onEditClicked: function(deckId) {
            pageLoader.setSource(pages.editDeck, { "deckId": deckId })
        }

        onExportClicked: function(deckId) {
            exportAnkiFolderDialog.deckId = deckId
            exportAnkiFolderDialog.open()
        }
    }

    QuestionPopup {
        id: deckRemoveQuestionPopup

        property int deckId: 0

        onAcceptClicked: {
            deckService.remove_deck(deckId)
            infoPopup.open(qsTr("Удалено"))
        }
    }

    Rectangle {
        id: loaderScreen
        color: "black"
        visible: pageLoader.status === Loader.Loading
        anchors.fill: parent
    }

    BusyIndicator {
        anchors.centerIn: parent
        width: 128
        height: 128
        running: deckService.importInProgress || deckService.exportInProgress || pageLoader.loading
    }

    Connections {
        target: deckService

        function onDeckImported(cardsImported) {
            infoPopup.open(qsTr("Импортировано карточек: " + cardsImported))
        }

        function onDeckUpdated() {
            infoPopup.open(qsTr("Колода обновлена"))
        }

        function onCardRemoved() {
            infoPopup.open(qsTr("Карточка удалена"))
        }

        function onDeckExported() {
            infoPopup.open(qsTr("Колода экспортирована"))
        }
    }

    Connections {
        id: popupServiceConnection
        target: popupService

        function onRequested(req) {
            if (req.type === PopupType.INFO) {
                infoPopup.open(req.message)
            } else {
                console.warn("Unknown popup type")
            }
        }
    }

    Connections {
        id: popupConnection
        target: infoPopup

        function onClosed() {
            let action = new PopupAction("closed")
            let response = new PopupResponse(action)

            popupService.response(response)
        }
    }
}

