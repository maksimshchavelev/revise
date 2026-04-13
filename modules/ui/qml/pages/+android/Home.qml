// Home page
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Effects
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property string windowTitle: qsTr("Revise")
    property bool openedAsWindow: false

    Revise.Background {
        id: background
        anchors.fill: parent
    }

    Revise.BlurOverlay {
        id: blurOverlay
        anchors.fill: parent
        sourceItem: background
        blur: 1.4
    }

    Revise.Toolbar {
        id: toolbar
        anchors.bottom: root.bottom
    }

    ColumnLayout {
        id: layout

        anchors {
            left: root.left
            top: root.top
            bottom: toolbar.top
            right: root.right
        }

        // Decks
        ListView {
            id: listView

            model: decksModel
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 6

            spacing: 12

            delegate: DelegateChooser {
                role: "isSpecial"

                DelegateChoice {
                    roleValue: true

                    delegate: Revise.AddDeckDelegate {
                        backgroundItem: blurOverlay.blurredItem

                        onImportClicked: importDialog.open()
                        onCreateClicked: root.createDeckPage()
                    }
                }

                DelegateChoice {
                    roleValue: false

                    delegate: Revise.DeckDelegate {
                        deckName: model.name
                        deckDescription: model.description
                        timeLimit: model.timeLimit
                        newCards: model.newCards
                        consolidateCards: model.consolidateCards
                        incorrectCards: model.incorrectCards
                        deckId: model.deckId
                        repeatableToday: model.repeatableToday
                        backgroundItem: blurOverlay.blurredItem

                        onStudyClicked: {
                            studyService.start(deckId)
                            router.navigate("training", {})
                        }

                        onEditClicked: {
                            router.navigate("deckEditor", {
                                                "deck": deckService.deck(deckId)
                                            }, Revise.page.Page)
                        }

                        onRemoveClicked: {
                            deckService.remove_deck(deckId)
                        }

                        onExportClicked: {
                            exportDialog.deckId = deckId
                            exportDialog.open()
                        }
                    }
                }
            }

            function iterateDelegates() {
                for (var i = 0; i < listView.contentItem.children.length; i++) {
                    var child = listView.contentItem.children[i]
                    if (child && typeof child.remapPosition === "function") {
                        child.remapPosition()
                    }
                }
            }

            onContentXChanged: iterateDelegates()
            onContentYChanged: iterateDelegates()
        }
    }

    function createDeckPage() {
        router.navigate("createDeck", {}, Revise.page.Page)
    }

    FileDialog {
        id: importDialog

        title: qsTr("Выберите файл")
        onAccepted: deckService.import_deck_async(selectedFile)

        nameFilters: [qsTr("Колода Revise (*.rpkg)"), qsTr(
                "Колода Anki (*.apkg)")]
    }

    FolderDialog {
        id: exportDialog

        property int deckId: 0

        title: qsTr("Экспортировать как .rpkg")
        onAccepted: deckService.export_deck_async(deckId, selectedFolder)
    }

    Shortcut {
        sequence: "Ctrl+N"
        onActivated: root.createDeckPage()
    }

    Shortcut {
        sequence: "Ctrl+I"
        onActivated: importDialog.open()
    }
}
