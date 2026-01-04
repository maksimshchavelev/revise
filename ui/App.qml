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
        "createCard": "qrc:/ui/pages/CreateCard.qml"
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
            if (item.back) {
                item.back.connect(function() {
                    pageLoader.source = pages.main
                })
            }
            if (item.addCardClicked) {
                item.addCardClicked.connect(function(deckId) {
                    pageLoader.setSource(pages.createCard, {
                                             "deckId": deckId
                                         })
                })
            }
            if (item.createClicked) {
                item.createClicked.connect(function(deckId, front, back) {
                    deckService.create_card(deckId, front, back)
                    infoPopup.open(qsTr("Карточка создана"))
                })
            }
            if (item.backClicked) {
                item.backClicked.connect(function() {
                    // This is a bit of a hack, we should use a router
                    // to go back to the previous page.
                    pageLoader.setSource(pages.editDeck, {
                                             "deckId": item.deckId
                                         })
                })
            }
            if (item.updateClicked) {
                item.updateClicked.connect(deckService.update_deck)
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
            deckService.remove_deck(deckId)
            infoPopup.open(qsTr("Удалено"))
        }

        onEditClicked: function(deckId) {
            pageLoader.setSource(pages.editDeck, { "deckId": deckId })
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        width: 128
        height: 128
        running: deckService.importInProgress
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

