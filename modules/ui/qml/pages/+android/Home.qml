// Home page
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Controls
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
            Layout.bottomMargin: 2

            ScrollBar.vertical: Revise.Scrollbar {
                policy: ScrollBar.AlwaysOn
            }

            spacing: 12

            delegate: Item {
                width: ListView.view.width
                height: delegateLoader.height

                Loader {
                    id: delegateLoader
                    sourceComponent: model.isSpecial ? addDeckDelegate : deckDelegate
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 8
                    anchors.rightMargin: 16
                }

                Component {
                    id: addDeckDelegate
                    Revise.AddDeckDelegate {
                        onImportClicked: importDialog.open()
                        onCreateClicked: root.createDeckPage()
                    }
                }

                Component {
                    id: deckDelegate
                    Revise.DeckDelegate {
                        deckName: model.name
                        deckDescription: model.description
                        timeLimit: model.timeLimit
                        newCards: model.newCards
                        consolidateCards: model.consolidateCards
                        incorrectCards: model.incorrectCards
                        deckId: model.deckId
                        repeatableToday: model.repeatableToday

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
