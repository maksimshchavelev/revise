// Home page

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import Revise as Revise

Item {
    id: root

    property var pageParams: null

    ColumnLayout {
        id: layout
        anchors.fill: root

        // Header
        Revise.Header {
            id: header

            Layout.fillWidth: true
            Layout.preferredHeight: 70

            onAddDeckClicked: addDeckDialog.open()
        }

        // Decks
        ListView {
            model: decksModel
            spacing: 10
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: 5
            Layout.leftMargin: 5

            delegate: Item {
                width: ListView.view.width
                height: childrenRect.height

                Revise.DeckDelegate {
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter

                    deckName: model.name
                    isTimeLimited: model.timeLimit > 0
                    newCards: model.newCards
                    consolidateCards: model.consolidateCards
                    incorrectCards: model.incorrectCards
                    deckId: model.deckId
                    repeatableToday: model.repeatableToday

                    onClicked: deckEventDialog.open(deckId)
                }
            }
        }
    }

    Revise.AddDeckDialog {
        id: addDeckDialog
        backgroundItem: layout

        onImportClicked: importDialog.open()
        onCreateClicked: createDeckDialog.open()
    }

    Revise.CreateDeckDialog {
        id: createDeckDialog
        backgroundItem: layout

        onCreateClicked: function(deck) {
            deckService.create_deck(deck)
        }
    }

    Revise.DeckEventDialog {
        id: deckEventDialog
        backgroundItem: layout

        onTrainClicked: function(deckId) {
            studyService.start(deckId)
            router.navigate("training", {})
        }

        onRemoveClicked: function(deckId) {
            deckService.remove_deck(deckId)
        }

        onEditClicked: function(deckId) {
            router.navigate("deckEditor", {"deck": deckService.deck(deckId)})
        }

        onExportClicked: function(deckId) {
            exportDialog.deckId = deckId
            exportDialog.open()
        }
    }

    FileDialog {
        id: importDialog

        title: qsTr("Выберите файл")
        onAccepted: deckService.import_deck_async(selectedFile)

        nameFilters: [qsTr("Колода Revise (*.rpkg)"), qsTr(
                "Колода Anki (*.apkg")]
    }

    FolderDialog {
        id: exportDialog

        property int deckId: 0

        title: qsTr("Сохранить .rpkg")

        onAccepted: deckService.export_deck_async(deckId, selectedFolder)
    }
}
