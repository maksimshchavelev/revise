// Card creation page
import QtQuick
import QtQuick.Layouts
import "../theme"
import "../controls"
import "../dialogs"

Item {
    id: root

    property int deckId: 0

    signal createClicked(int deckId, string front, string back)
    signal previewClicked(string front, string back)
    signal backClicked()

    function exitClicked() {
        root.backClicked()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        AppText {
            text: qsTr("Текст спереди")
        }

        ValidatedTextField {
            id: cardFront
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            placeholderText: qsTr("Текст спереди")
            inputMethodHints: Qt.ImhNoAutoUppercase
            onTextChanged: {
                valid = !deckService.is_card_exists(root.deckId, cardFront.text)
            }
        }

        RowLayout {
            Layout.fillWidth: true

            AppText {
                text: qsTr("Текст сзади")
            }

            Item {
                Layout.fillWidth: true
            }

            // Tool buttons
            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 32

                Button {
                    id: addMathButton
                    text: "f(x)"
                    anchors.fill: parent
                    onClicked: {
                        addFormulaPopup.open()
                    }
                }
            }

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 32

                Button {
                    id: addBoldButton
                    text: "<b>B</b>"
                    anchors.fill: parent
                    onClicked: {
                        wrapPopup.buttonText = qsTr("Сделать <b>жирным</b>")
                        wrapPopup.wrapLeft = "<b>"
                        wrapPopup.wrapRight = "</b>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }
                }
            }

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 32

                Button {
                    id: addItalicButton
                    text: "<i>I</i>"
                    anchors.fill: parent
                    onClicked: {
                        wrapPopup.buttonText = qsTr("Выделить <b>курсивом</b>")
                        wrapPopup.wrapLeft = "<i>"
                        wrapPopup.wrapRight = "</i>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }
                }
            }

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 32

                Button {
                    id: addUnderlinedButton
                    text: "<u>U</u>"
                    anchors.fill: parent
                    onClicked: {
                        wrapPopup.buttonText = qsTr("<b>Подчеркнуть</b>")
                        wrapPopup.wrapLeft = "<i>"
                        wrapPopup.wrapRight = "</i>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }
                }
            }
        }

        ValidatedMultilineTextField {
            id: cardBack
            Layout.fillWidth: true
            Layout.fillHeight: true
            inputMethodHints: Qt.ImhNoAutoUppercase
            placeholderText: qsTr("Текст сзади")
        }

        RowLayout {
            spacing: 5
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                Button {
                    id: btnAdd
                    anchors.fill: parent
                    text: qsTr("Добавить")
                    clickable: cardFront.valid && cardFront.text.trim() !== ""
                               && cardBack.valid && cardBack.text.trim() !== ""
                    onClicked: {
                        Qt.inputMethod.hide()
                        createClicked(root.deckId, cardFront.text,
                                      cardBack.text)
                        // Clear fields after adding
                        cardFront.text = ""
                        cardBack.text = ""
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                Button {
                    id: btnPreview
                    anchors.fill: parent
                    text: qsTr("Превью")
                    clickable: cardFront.valid && cardFront.text.trim() !== ""
                               && cardBack.valid && cardBack.text.trim() !== ""
                    onClicked: {
                        previewClicked(cardFront.text, cardBack.text)
                    }
                }
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.topMargin: 4
            text: qsTr("Назад")
            onClicked: {
                Qt.inputMethod.hide()
                root.backClicked()
            }
        }
    }

    WrapTextPopup {
        id: wrapPopup

        onWrapClicked: function(wrapped) {
            cardBack.rawTextEdit.insert(cardBack.rawTextEdit.cursorPosition, wrapped)
            wrapPopup.close()
        }
    }

    AddFormulaPopup {
        id: addFormulaPopup

        onAddInline: {
            wrapPopup.buttonText = qsTr("Добавить формулу")
            wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
            wrapPopup.wrapLeft = "\\("
            wrapPopup.wrapRight = "\\)"
            wrapPopup.open()
        }

        onAddDisplay: {
            wrapPopup.buttonText = qsTr("Добавить формулу")
            wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
            wrapPopup.wrapLeft = "\\["
            wrapPopup.wrapRight = "\\]"
            wrapPopup.open()
        }
    }
}
