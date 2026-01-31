// Card creation page
import QtQuick
import QtQuick.Layouts
import Revise

Item {
    id: root

    property int deckId: 0

    signal createClicked(int deckId, string front, string back)
    signal previewClicked(string front, string back)
    signal backClicked

    Shortcut {
        sequence: "Escape"
        onActivated: exitClicked()
    }

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

            Shortcut {
                sequence: "Ctrl+Return" // Return = Enter
                onActivated: {
                    cardFront.rawTextInput.focus = false
                    cardBack.rawTextEdit.focus = true
                    cardBack.rawTextEdit.forceActiveFocus()
                }
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

            Flow {
                Layout.fillWidth: true
                spacing: 5
                layoutDirection: Qt.RightToLeft

                // Tool buttons
                Button {
                    id: addMathButton
                    text: "f(x)"
                    onClicked: {
                        addFormulaPopup.open()
                    }
                    implicitWidth: 48
                    implicitHeight: 32
                }

                Button {
                    id: addBoldButton
                    text: "<b>B</b>"
                    onClicked: {
                        wrapPopup.buttonText = qsTr("Сделать <b>жирным</b>")
                        wrapPopup.wrapLeft = "<b>"
                        wrapPopup.wrapRight = "</b>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }

                    implicitWidth: 48
                    implicitHeight: 32
                }

                Button {
                    id: addItalicButton
                    text: "<i>I</i>"
                    onClicked: {
                        wrapPopup.buttonText = qsTr("Выделить <b>курсивом</b>")
                        wrapPopup.wrapLeft = "<i>"
                        wrapPopup.wrapRight = "</i>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }

                    implicitWidth: 48
                    implicitHeight: 32
                }

                Button {
                    id: addUnderlinedButton
                    text: "<u>U</u>"
                    onClicked: {
                        wrapPopup.buttonText = qsTr("<b>Подчеркнуть</b>")
                        wrapPopup.wrapLeft = "<i>"
                        wrapPopup.wrapRight = "</i>"
                        wrapPopup.textPlaceholder = qsTr("Введите текст")
                        wrapPopup.open()
                    }
                    implicitWidth: 48
                    implicitHeight: 32
                }

                Button {
                    id: addCodeButton
                    text: "code"
                    onClicked: {
                        addCodePopup.open()
                    }
                    implicitWidth: 48
                    implicitHeight: 32
                }
            }
        }

        ValidatedMultilineTextField {
            id: cardBack
            Layout.fillWidth: true
            Layout.fillHeight: true
            inputMethodHints: Qt.ImhNoAutoUppercase
            placeholderText: qsTr("Текст сзади")

            Shortcut {
                sequence: "Ctrl+B"
                onActivated: cardBack.wrapSelection("<b>", "</b>")
            }

            Shortcut {
                sequence: "Ctrl+I"
                onActivated: cardBack.wrapSelection("<i>", "</i>")
            }

            Shortcut {
                sequence: "Ctrl+U"
                onActivated: cardBack.wrapSelection("<u>", "</u>")
            }

            Shortcut {
                sequence: "Ctrl+F"
                onActivated: addFormulaPopup.open()
            }

            function wrapSelection(left, right) {
                var start = rawTextEdit.selectionStart
                var end = rawTextEdit.selectionEnd
                var selected = rawTextEdit.selectedText

                if (selected.length === 0)
                    return

                // build new text using rawTextEdit.text, not parent's text
                var newText = rawTextEdit.text.substring(
                            0,
                            start) + left + selected + right + rawTextEdit.text.substring(
                            end)

                rawTextEdit.text = newText
                rawTextEdit.cursorPosition = start + (left + selected + right).length
                rawTextEdit.deselect()
            }
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
                        Qt.inputMethod.hide()
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

        onWrapClicked: function (wrapped) {
            cardBack.rawTextEdit.focus = true
            cardBack.rawTextEdit.forceActiveFocus()
            cardBack.rawTextEdit.cursorVisible = true
            cardBack.rawTextEdit.insert(cardBack.rawTextEdit.cursorPosition,
                                        wrapped)
            wrapPopup.close()
        }
    }

    AddFormulaPopup {
        id: addFormulaPopup

        onAddInline: {
            wrapPopup.buttonText = qsTr("Добавить формулу")
            wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
            wrapPopup.wrapLeft = "\\("
            wrapPopup.wrapRight = "\\) "
            wrapPopup.open()
        }

        onAddDisplay: {
            wrapPopup.buttonText = qsTr("Добавить формулу")
            wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
            wrapPopup.wrapLeft = "\\["
            wrapPopup.wrapRight = "\\] "
            wrapPopup.open()
        }
    }

    AddCodePopup {
        id: addCodePopup

        onAddClicked: function (code, language) {
            cardBack.rawTextEdit.insert(cardBack.rawTextEdit.cursorPosition,
                                        `<pre><code class="${language.trim(
                                            )}">${code.trim()}</code></pre>`)
            addCodePopup.close()
        }
    }
}
