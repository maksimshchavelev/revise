// A modal dialog box that allows you to choose what to do with the deck. The user can modify it, start training, or
// delete it.

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Revise.PopupBase {
    id: root

    widthLimit: Math.min(parent.width * 0.85, 320)

    // Deck options
    property real deckId: 0

    signal trainClicked(int deckId)
    signal editClicked(int deckId)
    signal removeClicked(int deckId)
    signal exportClicked(int deckId)

    Revise.Button {
        text: qsTr("Тренировать")
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.topMargin: 15
        onClicked: {
            root.close()
            root.trainClicked(root.deckId)
        }
    }

    Revise.Button {
        text: qsTr("Редактировать")
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        onClicked: {
            root.close()
            root.editClicked(root.deckId)
        }
    }

    Revise.Button {
        text: qsTr("Экспортировать в .rpkg")
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        onClicked: {
            root.close()
            root.exportClicked(root.deckId)
        }
    }

    Revise.Button {
        text: qsTr("Удалить")
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.bottomMargin: 15
        onClicked: {
            root.close()
            root.removeClicked(root.deckId)
        }
    }

    function open(deckId) {
        root.deckId = deckId
        root._openBase()
    }

    function close() {
        root._closeBase()
    }
}
