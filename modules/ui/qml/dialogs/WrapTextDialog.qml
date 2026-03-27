
// This popup will wrap the text in the necessary HTML tags.
import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise as Revise

Revise.PopupBase {
    id: root

    widthLimit: parent.width * 0.85

    property string textPlaceholder: qsTr("Введите текст") // The Placeholder will be displayed in the text input field.
    property string buttonText
    // Button text
    property string text: textInput.text // Entered text
    property string wrapLeft: "" // What should I wrap the text in (left)?
    property string wrapRight: "" // What should I wrap the text in (left)?

    signal wrapClicked(string wrapped)
    signal changed(string text)

    // Text input
    Revise.ValidatedTextField {
        id: textInput
        Layout.fillWidth: true
        Layout.preferredHeight: 35
        placeholderText: root.textPlaceholder
        inputMethodHints: Qt.ImhNoAutoUppercase
        valid: false
        onTextChanged: {
            valid = textInput.text.length > 0
            preview.html = root.wrapLeft + textInput.text + root.wrapRight
            root.changed(textInput.text)
        }
    }

    Revise.HtmlView {
        id: preview
        Layout.fillWidth: true
        Layout.preferredHeight: 55
    }

    // Footer
    Revise.Button {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.topMargin: 4
        text: root.buttonText
        clickable: textInput.valid
        onClicked: {
            wrapClicked(root.wrapLeft + textInput.text + root.wrapRight)
            root.close()
        }
    }

    // Public API
    function open() {
        textInput.rawTextInput.focus = true
        textInput.rawTextInput.cursorVisible = true
        root._openBase()
    }

    function close() {
        textInput.text = ""
        textInput.rawTextInput.focus = false
        textInput.rawTextInput.cursorVisible = false
        root._closeBase()
    }
}
