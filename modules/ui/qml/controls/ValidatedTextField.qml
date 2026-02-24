// Single-line text input field. You can set a validator. Multicolored border is present.

import QtQuick
import QtQuick.Controls
import Revise

Item {
    id: root

    // Public properties
    property alias text: textInput.text
    property alias placeholderText: placeholder.text
    property alias validator: textInput.validator
    property alias maximumLength: textInput.maximumLength
    property alias echoMode: textInput.echoMode
    property alias inputMethodHints: textInput.inputMethodHints
    property bool valid: validator ? textInput.acceptableInput : true
    property bool editable: true
    property TextInput rawTextInput: textInput

    // Signals
    signal editingFinished()
    signal accepted()

    // Visual properties
    property int borderWidth: 2
    property color defaultBorderColor: Theme.textInputDefaultBorder
    property color validBorderColor: Theme.textInputValidBorder
    property color invalidBorderColor: Theme.textInputInvalidBorder
    property color backgroundColor: "black"
    property color textColor: Theme.textColor
    property color placeholderColor: Theme.textColorDark
    property int fontSize: Theme.textSizeDefault
    property int padding: 8

    // Private state
    readonly property color currentBorderColor: {
        if (textInput.length === 0) {
            return defaultBorderColor
        } else if (valid) {
            return validBorderColor
        } else {
            return invalidBorderColor
        }
    }
    // ... rest of file
    

    // Component dimensions
    width: 200
    height: 40

    // Background
    Rectangle {
        anchors.fill: parent
        radius: 4
        color: backgroundColor
        border.width: borderWidth
        border.color: currentBorderColor
        opacity: 0.4
    }

    Item {
        id: contentContainer
        anchors.fill: parent
        anchors.leftMargin: padding
        anchors.rightMargin: padding
        anchors.topMargin: padding
        anchors.bottomMargin: padding

        // Placeholder text (visible when input is empty)
        Text {
            id: placeholder
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            color: placeholderColor
            font.pixelSize: fontSize
            visible: textInput.text.length === 0 && !textInput.activeFocus
            elide: Text.ElideRight
        }

        // Main text input
        TextInput {
            id: textInput
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            readOnly: !root.editable

            // Visual properties
            color: textColor
            font.pixelSize: fontSize

            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0

            clip: true

            Keys.onPressed: function(event) {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    event.accepted = true
                    focus = false
                    root.accepted()
                }
            }

            // Signals
            onEditingFinished: {
                root.editingFinished()
                focus = false
            }
        }
    }

    MouseArea {
        visible: !textInput.focus
        anchors.fill: parent
        onClicked: textInput.focus = true
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.4
        visible: !root.editable
        radius: 4

        MouseArea {
            anchors.fill: parent
            onClicked: {}
            onPressed: {}
            onReleased: {}
        }
    }

    DebugBounds {}
}
