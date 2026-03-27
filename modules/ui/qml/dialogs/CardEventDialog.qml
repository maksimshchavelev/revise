// A dialog that displays what to do with the card

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Revise as Revise

Revise.PopupBase {
    id: root

    signal removeClicked(int cardId)
    signal editClicked(int cardId)
    signal previewClicked(int cardId)

    property int cardId: 0

    Revise.Button {
        text: qsTr("Превью")
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 20
        onClicked: {
            root.close()
            root.previewClicked(root.cardId)
        }
    }

    Revise.Button {
        text: qsTr("Редактировать")
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        onClicked: {
            root.close()
            root.editClicked(root.cardId)
        }
    }

    Revise.Button {
        text: qsTr("Удалить")
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Layout.bottomMargin: 20
        onClicked: {
            root.close()
            root.removeClicked(root.cardId)
        }
    }

    function open(cardId) {
        root.cardId = cardId
        root._openBase()
    }

    function close() {
        root._closeBase()
    }
}
