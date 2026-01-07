import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Dialogs
import QtQuick.Layouts
import Revise
import "navigation"
import "dialogs"
import "views/decks"
import "theme"
import "pages"

ApplicationWindow {
    id: appWindow
    visible: true
    background: Image {
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/res/img/background_blurred.png"
    }

    readonly property var pages: {
        "main": "qrc:/ui/MainLayout.qml",
        "training": "qrc:/ui/pages/Training.qml",
        "editDeck": "qrc:/ui/pages/EditDeck.qml",
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: pages.main

        onLoaded: {
            if (item.addDeckClicked) {
                item.addDeckClicked.connect(addDeckPopup.open)
            }
            if (item.deckClicked) {
                item.deckClicked.connect(function(deckId, repeatableToday) {
                    deckModalDialog.open(deckId, repeatableToday)
                })
            }
            if (item.trainingFinished) {
                item.trainingFinished.connect(function() {
                    pageLoader.source = pages.main
                    infoPopup.open(qsTr("Тренировка завершена!"))
                })
            }
            if (item.backClicked) {
                item.backClicked.connect(function() {
                    pageLoader.source = pages.main
                })
            }
        }
    }

    CreateDeckPopup {
        id: createDeckPopup
        onCreateClicked: function(name,
                                  description,
                                  timeLimit,
                                  newLimit,
                                  consolidateLimit,
                                  incorrectLimit) {
            deckService.create_deck(name, description, timeLimit, newLimit, consolidateLimit, incorrectLimit)
        }
    }

    InfoPopup {
        id: infoPopup
        sourceItem: pageLoader
    }

    ScrollableInfoPopup {
        id: scrollableInfoPopup
        sourceItem: pageLoader
    }

    FileDialog {
        id: importAnkiFileDialog
        title: qsTr("Выберите файл")
        nameFilters: [
            qsTr("Колода Anki (*.apkg)")
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
        sourceItem: pageLoader

        onRejected: console.log("Rejected")
        onImportAnkiSelected: importAnkiFileDialog.open()
        onCreateSelected: createDeckPopup.open()
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

    BusyIndicator {
        anchors.centerIn: parent
        width: 128
        height: 128
        running: deckService.importInProgress || deckService.exportInProgress
    }

    Connections {
        target: errorReporter

        function onError_occured(error) {
            scrollableInfoPopup.open(error.message + "\n\nDetails: " + error.details + "\n\nStacktrace:\n\n" + error.stacktrace)
        }
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

    onClosing: function(close) {
        close.accepted = false

        if (pageLoader.item && typeof pageLoader.item.exitClicked === 'function') {
            pageLoader.item.exitClicked()
        } else {
            console.warn("Failed to check pageLoader.item && typeof pageLoader.item.exitClicked === 'function'")
        }
    }
}

