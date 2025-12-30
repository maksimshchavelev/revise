import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Dialogs
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
            streak: streakBackend.streak
            updatedToday: streakBackend.isUpdatedToday
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

        SwipeView {
            id: pages
            anchors.fill: parent
            currentIndex: toolbar.currentIndex
            interactive: false

            Home {
                // The user clicked on the card and needs to start training.
                onDeckClicked: function(deckId) {
                    deckModalDialog.open(deckId)
                }

                onDeckIsUnrepeatableToday: function(deckName) {
                    infoPopup.open(qsTr("Колоду \"" + deckName + "\" нельзя повторить сегодня!"))
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
        onAccepted: decksIO.import_anki_deck_async(selectedFile)
    }

    AddDeckPopup {
        id: addDeckPopup
        sourceItem: content

        onRejected: console.log("Rejected")
        onImportAnkiSelected: importAnkiFileDialog.open()
        onCreateSelected: infoPopup.open(qsTr("Пока такой функционал не реализован :("))
    }

    DeckModalDialog {
        id: deckModalDialog
        sourceItem: content

        onTrainClicked: {
            header.visible = false
            footer.visible = false
            pages.currentIndex = 2 // switch to Training{}
            studyController.start_training(deckId)
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        width: 128
        height: 128
        running: decksIO.inProgress
    }

    Connections {
        target: errorReporter

        function onError_occured(error) {
            scrollableInfoPopup.open(error.message + "\n\nDetails: " + error.details + "\n\nStacktrace:\n\n" + error.stacktrace)
        }
    }

    Connections {
        target: decksIO

        function onImport_finished(cards) {
            infoPopup.open(qsTr("Импортировано карточек: " + cards))
        }
    }
}
