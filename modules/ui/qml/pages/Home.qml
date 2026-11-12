// Home.qml
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
        anchors.fill: root
    }

    GridLayout {
        anchors.fill: parent
        columns: Revise.SettingsService.useMobileUi ? 1 : 2
        rowSpacing: 0
        columnSpacing: 0

        Revise.Toolbar {
            id: toolbar

            Layout.row: Revise.SettingsService.useMobileUi ? 1 : 0
            Layout.column: 0
            Layout.fillWidth: Revise.SettingsService.useMobileUi
            Layout.fillHeight: !Revise.SettingsService.useMobileUi
            Layout.preferredWidth: Revise.SettingsService.useMobileUi ? -1 : 150
            Layout.preferredHeight: Revise.SettingsService.useMobileUi ? 60 : -1
        }

        Revise.DecksView {
            Layout.row: 0
            Layout.column: Revise.SettingsService.useMobileUi ? 0 : 1
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.leftMargin: 12
            Layout.rightMargin: Revise.SettingsService.useMobileUi ? 12 : 2
            Layout.topMargin: 2
            Layout.bottomMargin: 2

            onRemoveDeckClicked: function(deckId) {
                removeDeckDialog.deckId = deckId;
                removeDeckDialog.requestOpen();
            }

            onExportDeckClicked: function(deckId) {
                exportDialog.deckId = deckId;
                exportDialog.open();
            }

            onImportDeckClicked: importDialog.open()
            onCreateDeckClicked: createDeckDrawer.open()
        }
    }

    // Right-side panel to create deck
    Drawer {
        id: createDeckDrawer

        width: 350
        height: root.height
        edge: Qt.RightEdge
        scale: Revise.SettingsService.uiScale
        transformOrigin: Item.TopRight

        background: Revise.Background{}

        Overlay.modal: Rectangle {
            color: Qt.rgba(0, 0, 0, 0.4)
            anchors.fill: parent
        }

        enter: Transition {
            NumberAnimation {
                property: "position"
                from: 0.0
                to: 1.0
                duration: 280
                easing.type: Easing.OutCubic
            }
        }

        exit: Transition {
            NumberAnimation {
                property: "position"
                from: 1.0
                to: 0.0
                duration: 220
                easing.type: Easing.InCubic
            }
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

                ScrollBar.vertical: Revise.Scrollbar
                {
                }

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
                            control: Revise.Switch {
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

                            control: Revise.SpinBox {
                                id: deckTimeLimit

                                from: 0
                                to: 3600
                                editable: limitTime.checked
                                value: 0
                            }

                            Revise.HoverableTooltip {
                                anchors.fill: parent
                                text: qsTr("От 0 до 3600")
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение новых карточек")

                            control: Revise.SpinBox {
                                id: deckNewLimit

                                from: 1
                                to: 1000
                                value: 30

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: qsTr("От 1 до 1000")
                                }
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение повторяемых карточек")

                            control: Revise.SpinBox {
                                id: deckReviewLimit

                                from: 1
                                to: 1000
                                value: 30

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: qsTr("От 1 до 1000")
                                }
                            }
                        }

                        Revise.SettingsRow {
                            width: parent.width
                            text: qsTr("Ограничение ошибочных карточек")

                            control: Revise.SpinBox {
                                id: deckIncorrectLimit

                                from: 1
                                to: 1000
                                value: 30

                                Revise.HoverableTooltip {
                                    anchors.fill: parent
                                    text: qsTr("От 1 до 1000")
                                }
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
                clickable: deckName.textField.displayText.length > 0
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

    // ------------------- Action Dialogs -------------------
    Revise.ConfirmDialog {
        id: removeDeckDialog

        property int deckId

        titleText: qsTr("Удалить колоду?")
        messageText: qsTr("Колода будет удалена без возможности восстановления")
        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveDeck

        onAccepted: {
            Revise.DeckService.remove_deck(deckId)
            Revise.SettingsService.askRemoveDeck = !dontAskAgain
        }
    }
}
