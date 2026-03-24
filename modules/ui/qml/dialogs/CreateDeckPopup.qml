// Deck creation popup
import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise

PopupBase {
    id: root

    widthLimit: Math.min(parent.width * 0.85, 420)

    signal createClicked(Deck deck)

    // Deck name input
    ValidatedTextField {
        id: deckName
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Имя колоды")
        onTextChanged: {
            valid = !deckService.is_deck_exists(deckName.text)
        }
    }

    // Deck description
    ValidatedTextField {
        id: deckDescription
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Описание (необязательно)")
    }

    CheckupableOption {
        id: limitTime
        text: qsTr("Ограничить время ответа")
        Layout.fillWidth: true
        Layout.leftMargin: 3
    }

    // Time limit
    ValidatedTextField {
        id: deckTimeLimit
        editable: limitTime.checked
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Ограничение времени (сек)")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 3600
        }
    }

    // New limit
    ValidatedTextField {
        id: deckNewLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Максимум новых карточек")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    // Consolidate limit
    ValidatedTextField {
        id: deckConsolidateLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Максимум повторяемых")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    // Incorrect limit
    ValidatedTextField {
        id: deckIncorrectLimit
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: qsTr("Максимум ошибочных")
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator {
            bottom: 1
            top: 500
        }
    }

    Button {
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
            let deck = new Deck(deckName.text, deckDescription.text,
                                parseInt(deckTimeLimit.text),
                                parseInt(deckNewLimit.text),
                                parseInt(deckConsolidateLimit.text),
                                parseInt(deckIncorrectLimit.text))

            root.createClicked(deck)
            root.close()
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
