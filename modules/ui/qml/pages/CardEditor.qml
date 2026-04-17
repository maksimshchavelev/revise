// Page for editing or creating a new card.
import QtQuick
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

    Revise.SolidBackground {
        anchors.fill: parent
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        Revise.Text {
            text: qsTr("Текст спереди")
        }

        Revise.HtmlCardEditor {
            id: frontEditor
            Layout.fillWidth: true
            Layout.preferredHeight: 150

            Revise.LoadingScreen {
                id: frontLoadingScreen
                anchors.fill: parent
                thresholdTime: 0
            }
        }

        Revise.Text {
            text: qsTr("Текст сзади")
        }

        Revise.HtmlCardEditor {
            id: backEditor
            Layout.fillWidth: true
            Layout.fillHeight: true

            Revise.LoadingScreen {
                id: backLoadingScreen
                anchors.fill: parent
                thresholdTime: 0
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.margins: 5
            Layout.preferredHeight: 40

            Revise.Button {
                visible: !root.openedAsWindow
                text: qsTr("Назад")
                onClicked: router.back()
            }

            Revise.AcceptButton {
                text: pageParams && pageParams.editMode ? qsTr("Обновить") : qsTr("Добавить")
                onClicked: {
                    let frontHtml = ""
                    let backHtml = ""
                    let received = 0

                    function saveIfReady() {
                        if (received === 2) {
                            root.pageParams.card.front = frontHtml
                            root.pageParams.card.back = backHtml

                            if (root.pageParams.editMode === true) {
                                deckService.update_card(root.pageParams.card)
                            } else {
                                deckService.create_card(root.pageParams.card)
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
            }
        }
    }

    onPageParamsChanged: {
        if (!root.pageParams) {
            return
        }

        frontEditor.setHtml(root.pageParams.card.front)
        backEditor.setHtml(root.pageParams.card.back)
    }

    function onEnter() {}
    function onExit() {}
}
