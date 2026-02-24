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

    property string textPlaceholder: qsTr("Введите текст")  // The Placeholder will be displayed in the text input field.
    property string buttonText                              // Button text
    property string text: textInput.text                    // Entered text
    property string wrapLeft: ""                            // What should I wrap the text in (left)?
    property string wrapRight: ""                           // What should I wrap the text in (left)?

    signal wrapClicked(string wrapped)
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
        height: 170
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

                    // Text input
                    ValidatedTextField {
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

                    HtmlView {
                        id: preview
                        Layout.fillWidth: true
                        Layout.preferredHeight: 55
                    }
                }
            }

            // Footer
            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: root.buttonText
                clickable: textInput.valid
                onClicked:  {
                    wrapClicked(root.wrapLeft + textInput.text + root.wrapRight)
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
        textInput.rawTextInput.focus = true
        textInput.rawTextInput.cursorVisible = true
    }

    function close() {
        root.visible = false
        textInput.text = ""
        textInput.rawTextInput.focus = false
        textInput.rawTextInput.cursorVisible = false
        closed()
    }
}
