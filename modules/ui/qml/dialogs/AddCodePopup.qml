
// This popup This popup is needed to enter the code.

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise

PopupBase {
    id: root

    padding: 6

    property string textPlaceholder: qsTr("print(\"Hello world\")") // The Placeholder will be displayed in the text input field.
    property string text: textInput.text // Entered text

    signal addClicked(string code, string language)
    signal changed(string text)

    ValidatedTextField {
        id: langInput
        placeholderText: qsTr("Язык. Например, sql")
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        valid: false
        inputMethodHints: Qt.ImhNoAutoUppercase
        onTextChanged: {
            valid = langInput.text.length > 0
        }
    }

    // Text input
    ValidatedMultilineTextField {
        id: textInput
        Layout.fillWidth: true
        Layout.preferredHeight: 180
        placeholderText: root.textPlaceholder
        inputMethodHints: Qt.ImhNoAutoUppercase
        valid: false

        property string previousText: ""

        onTextChanged: {
            valid = textInput.text.length > 0
            root.changed(textInput.text)

            var cursorPos = rawTextEdit.cursorPosition

            if (text.length === previousText.length + 1) {
                var lastChar = rawTextEdit.text.charAt(cursorPos - 1)

                if (lastChar === '{')
                    rawTextEdit.insert(cursorPos, '}')
                if (lastChar === '(')
                    rawTextEdit.insert(cursorPos, ')')
                if (lastChar === '<')
                    rawTextEdit.insert(cursorPos, '>')
            }

            previousText = text
        }
    }

    // Footer
    Button {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.topMargin: 4
        text: qsTr("Добавить")
        clickable: textInput.valid && langInput.valid
        onClicked: {
            addClicked(textInput.text, langInput.text)
        }
    }

    // Public API
    function open() {
        langInput.rawTextInput.focus = true
        langInput.rawTextInput.cursorVisible = true
        _openBase()
    }

    function close() {
        textInput.text = ""
        langInput.text = ""
        textInput.rawTextEdit.focus = false
        textInput.rawTextEdit.cursorVisible = false
        langInput.rawTextInput.focus = false
        langInput.rawTextInput.cursorVisible = false
        _closeBase()
    }

    DebugBounds {}
}
