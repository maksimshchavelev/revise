import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Dialogs
import QtQuick.Layouts
import Revise
import "components"
import "pages"
import "qrc:/ui/theme"

ApplicationWindow {
    visible: true
    background: Image {
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/res/img/background_blurred.png"
    }

    header: Item {
        width: parent.width
        height: 70

        Rectangle {
            anchors.fill: parent
            color: Theme.toolbarColor
            opacity: 0.4
        }

        StreakIndicator {
            streak: streakService.streak
            updatedToday: streakService.updatedToday
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.margins: 15
        }

        ToolButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: 15
            activeIcon: "qrc:/res/img/add_deck.png"
            width: 36
            height: 36
            showBackground: false
            contentMargins: 0

            onClicked: addDeckPopup.open()
        }
    }

    footer: ToolBar {
        id: toolbar
        width: parent.width

        ToolButton {
            activeIcon: "qrc:/res/img/decks_selected.png"
            inactiveIcon: "qrc:/res/img/decks.png"
        }

        ToolButton {
            activeIcon: "qrc:/res/img/statistics_selected.png"
            inactiveIcon: "qrc:/res/img/statistics.png"
        }
    }

    Item {
        id: content
        anchors.fill: parent

        StackLayout {
            id: pages
            anchors.fill: parent
            currentIndex: toolbar.currentIndex

            Home {
                // The user clicked on the card and needs to start training.
                onDeckClicked: function(deckId, repeatableToday) {
                    deckModalDialog.open(deckId, repeatableToday)
                }
            }

            Statistics {}

            Training {
                onTrainingFinished: {
                    header.visible = true
                    footer.visible = true
                    pages.currentIndex = toolbar.currentIndex
                    infoPopup.open(qsTr("Тренировка завершена!"))
                }
            }

            EditDeck {
                id: editDeckPage

                onBack: {
                    header.visible = true
                    footer.visible = true
                    pages.currentIndex = toolbar.currentIndex
                }

                onUpdateClicked: function(id,
                                          name,
                                          description,
                                          timeLimit,
                                          newLimit,
                                          consolidateLimit,
                                          incorrectLimit) {
                    deckService.update_deck(id, name, description, timeLimit, newLimit,
                                            consolidateLimit, incorrectLimit)
                }
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
        sourceItem: content
    }

    ScrollableInfoPopup {
        id: scrollableInfoPopup
        sourceItem: content
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
        sourceItem: content

        onRejected: console.log("Rejected")
        onImportAnkiSelected: importAnkiFileDialog.open()
        onCreateSelected: createDeckPopup.open()
    }

    DeckModalDialog {
        id: deckModalDialog
        sourceItem: content

        onTrainClicked: function(deckId, repeatableToday) {
            if (repeatableToday) {
                header.visible = false
                footer.visible = false
                pages.currentIndex = 2 // switch to Training{}
                studyService.start_training(deckId)
            } else {
                infoPopup.open(qsTr("Колоду нельзя повторить сегодня"))
            }
        }

        onRemoveClicked: function(deckId) {
            deckService.remove_deck(deckId)
            infoPopup.open(qsTr("Удалено"))
        }

        onEditClicked: function(deckId) {
            editDeckPage.load(deckId)
            pages.currentIndex = 3
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
}
