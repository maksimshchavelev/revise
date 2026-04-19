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
                id: delegateRoot
                width: ListView.view.width

                readonly property bool isSpecial: model.isSpecial

                height: isSpecial ? specialDelegate.height : deckDelegate.height

                Revise.AddDeckDelegate {
                    id: specialDelegate
                    visible: delegateRoot.isSpecial
                    width: delegateRoot.width - 24
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 16

                    onImportClicked: importDialog.open()
                    onCreateClicked: root.createDeckPage()
                }

                Revise.DeckDelegate {
                    id: deckDelegate
                    visible: !delegateRoot.isSpecial
                    width: delegateRoot.width - 24
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 16

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
                                            "deckId": deckId
                                        })
                    }
                    onRemoveClicked: deckService.remove_deck(deckId)
                    onExportClicked: {
                        exportDialog.deckId = deckId
                        exportDialog.open()
                    }
                }
            }
        }
    }

    function createDeckPage() {
        router.navigate("createDeck", {}, Revise.page.Page)
    }

    function onEnter() {}
    function onExit() {}

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
