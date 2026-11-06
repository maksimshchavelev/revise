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

        Revise.Flickable {
            id: cardFlickable

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 0

            contentWidth: width
            contentHeight: contentLayout.height

            ScrollBar.vertical: Revise.Scrollbar {
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

        Revise.HorizontalDivider {
            Layout.fillWidth: true
        }

        // Footer
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: Revise.Theme.surfaceColor
            radius: 8

            layer.effect: Revise.Shadow {}

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                anchors.topMargin: 10
                anchors.bottomMargin: 10
                spacing: 8

                // Use KaTeX tooltip
                Revise.IconText {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    icon.source: "qrc:/res/img/info.svg"
                    icon.size: 20

                    label.text: qsTr("Используйте KaTeX для формул")
                    label.font.bold: true
                    label.wrapMode: Revise.Text.NoWrap
                    label.elide: Revise.Text.ElideRight
                    label.maximumLineCount: 1
                }

                Revise.HorizontalSpacer {}

                Revise.Button {
                    text: qsTr("Назад")
                    onClicked: Revise.Router.back()
                }

                Revise.Button {
                    text: root.pageParams && root.pageParams.editMode ? qsTr("Обновить") : qsTr("Добавить")
                    backgroundItem.color: Revise.Theme.primaryColor
                    onClicked: root.save()
                }
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: root.save()
    }

    Timer {
        id: loadingTimer
        repeat: false
        running: false
        interval: 500
        onTriggered: {
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

        frontLoadingScreen.startLoading()
        backLoadingScreen.startLoading()

        loadingTimer.start()

        frontEditor.setHtml(root.pageParams.card.front)
        backEditor.setHtml(root.pageParams.card.back)
    }

    function onExit() {
        frontEditor.clear()
        backEditor.clear()
    }
}
