// Home page

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import QtQml.Models
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
        height: parent.height
    }

    // Decks
    GridView {
        id: gridView

        anchors {
            left: toolbar.right
            top: root.top
            bottom: root.bottom
            right: root.right
            leftMargin: 6
            rightMargin: 2
            bottomMargin: 2
            topMargin: 2
        }

        model: Revise.DecksModel
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        cellWidth: 180
        cellHeight: 220
        rightMargin: 6

        ScrollBar.vertical: Revise.Scrollbar {
            visible: gridView.contentHeight > gridView.height
        }

        delegate: DelegateChooser {
            role: "isSpecial"

            DelegateChoice {
                roleValue: true

                delegate: Revise.AddDeckDelegate
                {
                    onImportClicked: importDialog.open()
                    onCreateClicked: createDeckDrawer.open()
                }
            }

            DelegateChoice {
                roleValue: false

                delegate: Revise.DeckDelegate
                {
                    required property int newCards
                    required property int reviewCards
                    required property int incorrectCards
                    required property int deckId
                    required property int timeLimit
                    required property string name
                    required property string description
                    required property bool repeatableToday

                    deckNameProperty: name
                    deckDescriptionProperty: description
                    timeLimitProperty: timeLimit
                    newCardsProperty: newCards
                    reviewCardsProperty: reviewCards
                    incorrectCardsProperty: incorrectCards
                    deckIdProperty: deckId
                    repeatableTodayProperty: repeatableToday

                    onStudyClicked: {
                        Revise.StudyService.start(deckId)
                        Revise.Router.navigate("training", {})
                    }

                    onEditClicked: {
                        Revise.Router.navigate("deckEditor", {
                            "deckId": deckId
                        })
                    }

                    onRemoveClicked: {
                        Revise.DeckService.remove_deck(deckId)
                    }

                    onExportClicked: {
                        exportDialog.deckId = deckId
                        exportDialog.open()
                    }
                }
            }
        }
    }

    // Right-side panel to create deck
    Drawer {
        id: createDeckDrawer

        width: 350
        height: root.height
        edge: Qt.RightEdge

        background: Revise.Background {}

        Overlay.modal: Rectangle {
            color: Qt.rgba(0, 0, 0, 0.4)
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            Revise.Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true

                contentWidth: width
                contentHeight: deckCreateTab.implicitHeight

                ScrollBar.vertical: Revise.Scrollbar {}

                Column {
                    id: deckCreateTab

                    width: parent.width - 16
                    spacing: 16

                    // Panel showing deck primary options
                    Revise.SettingsPanel {
                        width: parent.width
                        header.icon.source: "qrc:/res/img/deck.svg"
                        header.label.text: qsTr("Основные параметры")

                        Revise.FormTextField {
                            id: deckName
                            width: parent.width
                            label.text: qsTr("Название колоды")
                            textField.placeholder.text: qsTr("Введите название колоды...")
                        }

                        Revise.FormTextField {
                            id: deckDescription
                            width: parent.width
                            label.text: qsTr("Описание колоды (необязательно)")
                            textField.placeholder.text: qsTr("Введите описание колоды...")
                        }
                    }

                    // Panel showing deck limits
                    Revise.SettingsPanel {
                        width: parent.width
                        header.icon.source: "qrc:/res/img/restrictions.svg"
                        header.label.text: qsTr("Ограничения")

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничить время ответа")
                            Revise.Switch {
                                id: limitTime
                                checked: false
                            }
                        }

                        Revise.HorizontalDivider {
                            width: parent.width
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Время ответа, сек")

                            Revise.SpinBox {
                                id: deckTimeLimit

                                minimum: 0
                                maximum: 3600
                                editable: limitTime.checked
                                placeholder: qsTr("От 0 до 3600")
                                value: 0
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение новых карточек")

                            Revise.SpinBox {
                                id: deckNewLimit

                                minimum: 1
                                maximum: 1000
                                placeholder: qsTr("От 1 до 1000")
                                value: 30
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение повторяемых карточек")

                            Revise.SpinBox {
                                id: deckReviewLimit

                                minimum: 1
                                maximum: 1000
                                placeholder: qsTr("От 1 до 1000")
                                value: 30
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение ошибочных карточек")

                            Revise.SpinBox {
                                id: deckIncorrectLimit

                                minimum: 1
                                maximum: 1000
                                placeholder: qsTr("От 1 до 1000")
                                value: 30
                            }
                        }
                    }
                }
            }

            // Create a new deck
            Revise.Button {
                Layout.fillWidth: true
                text: qsTr("Создать")
                backgroundItem.color: Revise.Theme.primaryColor
                clickable: deckName.textField.text.length > 0 && deckDescription.textField.text.length > 0
                onClicked: {
                    let draft = new Revise.deckDraft()

                    draft.name = deckName.textField.text
                    draft.description = deckDescription.textField.text
                    draft.timeLimit = limitTime.checked ? deckTimeLimit.value : 0
                    draft.newLimit = deckNewLimit.value
                    draft.reviewLimit = deckReviewLimit.value
                    draft.incorrectLimit = deckIncorrectLimit.value

                    Revise.DeckService.create_deck(draft)

                    createDeckDrawer.close()

                    deckName.textField.text = ""
                    deckDescription.textField.text = ""
                    deckTimeLimit.value = 0
                    deckNewLimit.value = 30
                    deckReviewLimit.value = 30
                    deckIncorrectLimit.value = 30
                }
            }
        }
    }

    function onEnter() {
    }

    function onExit() {
    }

    FileDialog {
        id: importDialog

        title: qsTr("Выберите файл")
        onAccepted: Revise.DeckService.import_deck(selectedFile)

        nameFilters: [qsTr("Колода Revise (*.rpkg)"), qsTr(
            "Колода Anki (*.apkg)")]
    }

    FolderDialog {
        id: exportDialog

        property int deckId: 0

        title: qsTr("Экспортировать как .rpkg")
        onAccepted: Revise.DeckService.export_deck(deckId, selectedFolder)
    }

    Shortcut {
        sequence: "Ctrl+N"
        onActivated: createDeckDrawer.open()
    }

    Shortcut {
        sequence: "Ctrl+I"
        onActivated: importDialog.open()
    }
}
