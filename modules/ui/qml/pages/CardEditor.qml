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
        }

        Revise.Text {
            text: qsTr("Текст сзади")
        }

        Revise.HtmlCardEditor {
            id: backEditor
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            Rectangle {
                anchors.fill: parent
                color: Revise.Theme.backgroundLight
                radius: 8
            }

            Row {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 8
                spacing: 4

                Revise.Button {
                    visible: !root.openedAsWindow
                    text: qsTr("Назад")
                    onClicked: router.back()
                }

                Revise.AcceptButton {
                    text: pageParams && pageParams.editMode ? qsTr("Обновить") : qsTr("Добавить")
                    onClicked: root.save()
                }
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: root.save()
    }

    function save() {
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

    function onEnter() {
        if (!root.pageParams) {
            return
        }

        frontEditor.setHtml(root.pageParams.card.front)
        backEditor.setHtml(root.pageParams.card.back)
    }

    function onExit() {
        frontEditor.clear()
        backEditor.clear()
    }
}
