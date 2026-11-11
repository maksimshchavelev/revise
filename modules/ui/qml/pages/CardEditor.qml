// CardEditor.qml
// Page for editing or creating a new card.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property var pageParams: null // contains editMode and Revise.Card (named "card")
    property bool openedAsWindow: false
    property string windowTitle: qsTr(`${pageParams && pageParams.editMode ? "Редактирование" : "Добавление"} карточки - Revise`)

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.background
    }


    Revise.Background {
        anchors.fill: parent
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 16

        // --------------------- Top toolbar ---------------------
        // Note: WebView will cover Toast notifications, so on Android, the panel is at the top
        Revise.CardEditorToolbar {
            id: upToolbar
            visible: Revise.BuildInfo.isMobileBuild
            isEditMode: root.pageParams && root.pageParams.editMode
            onSaveClicked: root.save()
        }

        Revise.HorizontalDivider {
            visible: Revise.BuildInfo.isMobileBuild
            Layout.fillWidth: true
        }
        // -------------------------------------------------------


        Revise.Flickable {
            id: cardFlickable

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 0

            contentWidth: width
            contentHeight: contentLayout.height

            ScrollBar.vertical: Revise.Scrollbar
            {
                visible: true
            }

            ColumnLayout {
                id: contentLayout

                width: cardFlickable.width - 18
                height: Math.max(cardFlickable.height, 400)

                spacing: 16

                // Card front editor
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: 150
                    Layout.maximumHeight: 300

                    spacing: 8

                    Revise.IconText {
                        id: frontLabel

                        Layout.fillWidth: true

                        icon.source: "qrc:/res/img/circle.svg"
                        icon.color: Revise.Theme.primaryColor
                        label.text: qsTr("Лицевая сторона")
                        label.font.bold: true
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumHeight: 0

                        Revise.HtmlCardEditor {
                            id: frontEditor
                            visible: Revise.BuildInfo.isMobileBuild
                            anchors.fill: parent
                        }

                        Revise.LoadingScreen {
                            id: frontLoadingScreen
                            anchors.fill: parent
                            minVisibleTime: 500
                            thresholdTime: 0
                            background.color: Revise.Theme.surfaceColor
                        }
                    }
                }

                // Card back editor
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: 250

                    spacing: 8

                    Revise.IconText {
                        id: backLabel

                        Layout.fillWidth: true

                        icon.source: "qrc:/res/img/circle.svg"
                        icon.color: Revise.Theme.primaryColor
                        label.text: qsTr("Обратная сторона")
                        label.font.bold: true
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumHeight: 0

                        Revise.HtmlCardEditor {
                            id: backEditor
                            visible: Revise.BuildInfo.isMobileBuild
                            anchors.fill: parent
                        }

                        Revise.LoadingScreen {
                            id: backLoadingScreen
                            anchors.fill: parent
                            minVisibleTime: 500
                            thresholdTime: 0
                            background.color: Revise.Theme.surfaceColor
                        }
                    }
                }
            }
        }


        // --------------------- Bottom toolbar ---------------------
        Revise.HorizontalDivider {
            visible: !Revise.BuildInfo.isMobileBuild
            Layout.fillWidth: true
        }

        Revise.CardEditorToolbar {
            id: bottomToolbar
            visible: !Revise.BuildInfo.isMobileBuild
            isEditMode: root.pageParams && root.pageParams.editMode
            onSaveClicked: root.save()
        }
        // -----------------------------------------------------------
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: root.save()
    }

    Timer {
        id: loadingTimer
        repeat: false
        running: false
        interval: 350
        onTriggered: {
            frontEditor.visible = true
            backEditor.visible = true

            frontLoadingScreen.endLoading()
            backLoadingScreen.endLoading()
        }
    }

    function save() {
        let frontHtml = ""
        let backHtml = ""
        let received = 0

        function saveIfReady() {
            if (received === 2) {
                root.pageParams.card.front = frontHtml
                root.pageParams.card.back = backHtml

                if (root.pageParams.editMode) {
                    let draft = new Revise.updateCardDraft()
                    draft.front = frontHtml
                    draft.back = backHtml
                    Revise.DeckService.update_cards([root.pageParams.card.id], draft)
                } else {
                    let draft = new Revise.createCardDraft(frontHtml, backHtml)
                    Revise.DeckService.create_card(root.pageParams.card.deckId, draft)
                    frontEditor.clear()
                    backEditor.clear()
                }
            }
        }

        frontEditor.getHtml(function (html) {
            frontHtml = html
            received++
            saveIfReady()
        })

        backEditor.getHtml(function (html) {
            backHtml = html
            received++
            saveIfReady()
        })
    }

    function onEnter() {
        if (!root.pageParams) {
            return
        }

        if (Revise.BuildInfo.isMobileBuild) {
            frontEditor.active = true
            backEditor.active = true
        } else {
            frontLoadingScreen.startLoading()
            backLoadingScreen.startLoading()
            loadingTimer.start()
        }

        frontEditor.setHtml(root.pageParams.card.front)
        backEditor.setHtml(root.pageParams.card.back)
    }

    function onExit() {
        frontEditor.clear()
        backEditor.clear()

        // Note: On Android, WebView may remain visible even after setting
        // `visible = false`, so you must explicitly destroy it
        if (Revise.BuildInfo.isMobileBuild) {
            frontEditor.active = false
            backEditor.active = false
        }
    }

    Shortcut {
        sequence: "Back"
        enabled: Revise.BuildInfo.isMobileBuild
        onActivated: Revise.Router.back()
    }
}
