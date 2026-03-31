// Page for editing or creating a new card.
import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property var pageParams: null // contains editMode and Revise.Card (named "card")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        Revise.AppText {
            text: qsTr("Текст спереди")
        }

        Revise.TextField {
            id: cardFront
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            placeholder.text: qsTr("Текст спереди")
            inputMethodHints: Qt.ImhNoAutoUppercase
            text: pageParams.card.front

            // onTextChanged: valid = !deckService.is_card_exists(root.deckId, cardFront.text)
            Shortcut {
                sequence: "Ctrl+Return"
                onActivated: {
                    cardFront.rawTextInput.focus = false
                    cardBack.rawTextEdit.focus = true
                    cardBack.rawTextEdit.forceActiveFocus()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Revise.AppText {
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
                Revise.Button {
                    id: addMathButton
                    text: "f(x)"
                    onClicked: {
                        addFormulaPopup.open()
                    }
                    implicitWidth: 48
                    implicitHeight: 32
                }

                Revise.Button {
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

                Revise.Button {
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

                Revise.Button {
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

                Revise.Button {
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

        Revise.ValidatedMultilineTextField {
            id: cardBack
            Layout.fillWidth: true
            Layout.fillHeight: true
            inputMethodHints: Qt.ImhNoAutoUppercase
            placeholderText: qsTr("Текст сзади")
            text: pageParams.card.back

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

                Revise.Button {
                    id: btnAdd
                    anchors.fill: parent
                    text: root.pageParams.editMode ? qsTr("Обновить") : qsTr(
                                                         "Добавить")

                    clickable: cardFront.valid && cardFront.text.trim() !== ""
                               && cardBack.valid && cardBack.text.trim() !== ""

                    onClicked: {
                        if (root.pageParams.editMode) {
                            root.pageParams.card.front = cardFront.text.trim()
                            root.pageParams.card.back = cardBack.text.trim()

                            deckService.update_card(root.pageParams.card)
                        } else {
                            root.pageParams.card.difficulty = 2.5
                            root.pageParams.card.front = cardFront.text.trim()
                            root.pageParams.card.back = cardBack.text.trim()

                            deckService.create_card(root.pageParams.card)
                        }

                        if (!pageParams.editMode) {
                            cardFront.text = ""
                            cardBack.text = ""
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                Revise.Button {
                    id: btnPreview
                    anchors.fill: parent
                    text: qsTr("Превью")
                    clickable: cardFront.valid && cardFront.text.trim() !== ""
                               && cardBack.valid && cardBack.text.trim() !== ""
                    onClicked: {
                        Qt.inputMethod.hide()

                        pageParams.card.front = cardFront.text
                        pageParams.card.back = cardBack.text

                        // Dear readers of this code! Here is a high-quality example of a workaround. We navigate to
                        // our own page by updating `pageParams` with a new card. In this case, when we return from
                        // `cardPreview`, the card text will load normally from the history.
                        router.navigate("cardEditor", pageParams)
                        router.navigate("cardPreview", {
                                            "card": pageParams.card
                                        })
                    }
                }
            }
        }

        Revise.Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.topMargin: 4
            text: qsTr("Назад")
            onClicked: {
                Qt.inputMethod.hide()
                router.back()
            }
        }

        Revise.VerticalSpacer {}
    }

    // Revise.WrapTextDialog {
    //     id: wrapPopup

    //     onWrapClicked: function (wrapped) {
    //         cardBack.rawTextEdit.focus = true
    //         cardBack.rawTextEdit.forceActiveFocus()
    //         cardBack.rawTextEdit.cursorVisible = true
    //         cardBack.rawTextEdit.insert(cardBack.rawTextEdit.cursorPosition,
    //                                     wrapped)
    //     }
    // }

    // Revise.AddFormulaPopup {
    //     id: addFormulaPopup

    //     onAddInline: {
    //         wrapPopup.buttonText = qsTr("Добавить формулу")
    //         wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
    //         wrapPopup.wrapLeft = "\\("
    //         wrapPopup.wrapRight = "\\) "
    //         wrapPopup.open()
    //     }

    //     onAddDisplay: {
    //         wrapPopup.buttonText = qsTr("Добавить формулу")
    //         wrapPopup.textPlaceholder = qsTr("Введите LaTeX")
    //         wrapPopup.wrapLeft = "\\["
    //         wrapPopup.wrapRight = "\\] "
    //         wrapPopup.open()
    //     }
    // }

    // Revise.AddCodeDialog {
    //     id: addCodePopup

    //     onAddClicked: function (code, language) {
    //         cardBack.rawTextEdit.insert(cardBack.rawTextEdit.cursorPosition,
    //                                     `<pre><code class="${language.trim(
    //                                         )}">${code.trim()}</code></pre>`)
    //     }
    // }

    onPageParamsChanged: console.log(root.pageParams.card, root.pageParams.card.deck_id)
}
