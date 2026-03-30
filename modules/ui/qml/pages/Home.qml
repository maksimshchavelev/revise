// Home page
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Effects
import Revise as Revise

Item {
    id: root

    property var pageParams: null

    Revise.Background {
        id: background
        anchors.fill: parent
    }

    ShaderEffectSource {
        id: bgSource
        sourceItem: background
        anchors.fill: parent
        live: true
        hideSource: false
        visible: false
    }

    MultiEffect {
        id: blurredBackground
        anchors.fill: parent
        source: bgSource
        blur: 1.5
        blurEnabled: true
        visible: false
    }

    ColumnLayout {
        id: layout
        anchors.fill: root

        // Decks
        GridView {
            id: gridView

            model: decksModel
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            cellWidth: 180
            cellHeight: 220

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5

            delegate: Revise.DeckDelegate {
                deckName: model.name
                deckDescription: model.description
                timeLimit: model.timeLimit
                newCards: model.newCards
                consolidateCards: model.consolidateCards
                incorrectCards: model.incorrectCards
                deckId: model.deckId
                repeatableToday: model.repeatableToday
                backgroundItem: blurredBackground

                onStudyClicked: {
                    studyService.start(deckId)
                    router.navigate("training", {})
                }

                onEditClicked: {
                    router.navigate("deckEditor", {
                                        "deck": deckService.deck(deckId)
                                    }, Revise.page.Window)
                }

                onRemoveClicked: {
                    deckService.remove_deck(deckId)
                }
            }

            function iterateDelegates() {
                for (var i = 0; i < gridView.contentItem.children.length; i++) {
                    var child = gridView.contentItem.children[i]
                    if (child && typeof child.remapPosition === "function") {
                        child.remapPosition()
                    }
                }
            }

            onContentXChanged: iterateDelegates()
            onContentYChanged: iterateDelegates()
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

        onCreateClicked: function (deck) {
            deckService.create_deck(deck)
        }
    }

    Revise.DeckEventDialog {
        id: deckEventDialog
        backgroundItem: layout

        onTrainClicked: function (deckId) {
            studyService.start(deckId)
            router.navigate("training", {})
        }

        onRemoveClicked: function (deckId) {
            deckService.remove_deck(deckId)
        }

        onEditClicked: function (deckId) {
            router.navigate("deckEditor", {
                                "deck": deckService.deck(deckId)
                            })
        }

        onExportClicked: function (deckId) {
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
