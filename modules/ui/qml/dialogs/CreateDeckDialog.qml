// Deck creation dialog

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Revise.PopupBase {
    id: root

    widthLimit: Math.min(parent.width * 0.85, 420)

    signal createClicked(Revise.Deck deck)

    // Deck name input
    Revise.TextField {
        id: deckName
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Имя колоды")
        onTextChanged: {
            valid = !deckService.is_deck_exists(deckName.text)
        }
    }

    // Deck description
    Revise.TextField {
        id: deckDescription
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Описание (необязательно)")
    }

    Revise.CheckBoxOption {
        id: limitTime
        text: qsTr("Ограничить время ответа")
        Layout.fillWidth: true
        Layout.leftMargin: 3
    }

    // Time limit
    Revise.TextField {
        id: deckTimeLimit
        editable: limitTime.checked
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Ограничение времени (сек)")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 3600
        }
    }

    // New limit
    Revise.TextField {
        id: deckNewLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Максимум новых карточек")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    // Consolidate limit
    Revise.TextField {
        id: deckConsolidateLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Максимум повторяемых")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    // Incorrect limit
    Revise.TextField {
        id: deckIncorrectLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholder.text: qsTr("Максимум ошибочных")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    Revise.Button {
        text: qsTr("Создать")
        clickable: deckName.valid && deckName.text !== ""
                   && (deckDescription.valid || deckDescription.text
                       === "") && (!limitTime.checked
                                   || (deckTimeLimit.valid && deckTimeLimit.text
                                       !== "")) && deckNewLimit.valid
                   && deckNewLimit.text !== ""
                   && deckConsolidateLimit.valid && deckConsolidateLimit.text
                   !== "" && deckIncorrectLimit.valid
                   && deckIncorrectLimit.text !== ""

        onClicked: {
            let deck = new Revise.Deck(deckName.text, deckDescription.text,
                                parseInt(deckTimeLimit.text),
                                parseInt(deckNewLimit.text),
                                parseInt(deckConsolidateLimit.text),
                                parseInt(deckIncorrectLimit.text))

            root.close()
            root.createClicked(deck)
        }
    }

    // Public API
    function open() {
        root._openBase()
    }

    function close() {
        deckName.text = ""
        deckDescription.text = ""
        deckTimeLimit.text = ""
        deckNewLimit.text = ""
        deckConsolidateLimit.text = ""
        deckIncorrectLimit.text = ""
        root._closeBase()
    }
}
