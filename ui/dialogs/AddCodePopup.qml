// This popup will wrap the text in the necessary HTML tags.

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property Item sourceItem
    property bool modal: true
    property real maxHeightRatio: 0.9

    property string textPlaceholder: qsTr("print(\"Hello world\")")  // The Placeholder will be displayed in the text input field.
    property string text: textInput.text                             // Entered text

    signal addClicked(string code, string language)
    signal changed(string text)
    signal closed()

    // Background blur effect
    ShaderEffectSource {
        id: snapshot
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: true
        recursive: true
        hideSource: false
        smooth: true
    }

    MultiEffect {
        anchors.fill: parent
        source: snapshot
        blurEnabled: true
        blur: 1
        blurMax: 32
        autoPaddingEnabled: false
    }

    // Dimming layer
    Rectangle {
        id: dimmer
        anchors.fill: parent
        color: "black"
        opacity: 0.35

        MouseArea {
            anchors.fill: parent
            enabled: root.modal && root.visible
            hoverEnabled: true
            onClicked: root.close()
            onPressed: {}
            onReleased: {}
        }
    }

    // Main popup container
    Item {
        id: popupContainer
        width: parent.width * 0.85
        height: 300
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        // Background panel
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.4
            radius: 20
            border.width: 2
            border.color: Theme.popupBorder
        }

        // Main column layout
        ColumnLayout {
            id: contentColumn
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            // Scrollable content area
            Flickable {
                id: flickable
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: width
                contentHeight: inputLayout.implicitHeight
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                // Input fields container
                ColumnLayout {
                    id: inputLayout
                    width: flickable.width
                    spacing: 8

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
                }
            }

            // Footer
            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Добавить")
                clickable: textInput.valid && langInput.valid
                onClicked:  {
                    addClicked(textInput.text, langInput.text)
                }
            }
        }

        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }
    }

    // Root animations
    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 100
        }
    }

    // Public API
    function open() {
        root.visible = true
        langInput.rawTextInput.focus = true
        langInput.rawTextInput.cursorVisible = true
    }

    function close() {
        root.visible = false
        textInput.text = ""
        langInput.text = ""
        textInput.rawTextEdit.focus = false
        textInput.rawTextEdit.cursorVisible = false
        langInput.rawTextInput.focus = false
        langInput.rawTextInput.cursorVisible = false
        closed()
    }
}
