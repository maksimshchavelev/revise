// Card creation popup

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import "../theme"
import "../controls"
import "../components"

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property Item sourceItem
    property bool modal: true
    property real maxHeightRatio: 0.85
    property int deckId: 0

    signal createClicked(int deckId,
                         string front,
                         string back)

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
        opacity: 0.7

        MouseArea {
            anchors.fill: parent
            enabled: root.modal && root.visible
            hoverEnabled: true
            onClicked: {}
            onPressed: {}
            onReleased: {}
        }
    }

    // Main popup container
    Item {
        id: popupContainer
        width: parent.width * 0.85
        height: parent.height * 0.85
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        // Background panel
        Rectangle {
            anchors.fill: parent
            color: "black"
            border.color: Theme.popupBorder
            border.width: 2
            opacity: 0.4
            radius: 20
        }

        // Main column layout
        ColumnLayout {
            id: contentColumn
            anchors.fill: parent
            anchors.margins: 8
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
                    height: flickable.height
                    spacing: 8

                    Item { Layout.fillWidth: true}

                    // Card front input
                    ValidatedTextField {
                        id: cardFront
                        Layout.fillWidth: true
                        Layout.preferredHeight: 35
                        placeholderText: qsTr("Текст спереди")
                        onTextChanged: {
                            valid = !deckService.is_card_exists(root.deckId, cardFront.text)
                        }
                    }

                    // Card back input
                    ValidatedMultilineTextField {
                        id: cardBack
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: qsTr("Текст сзади")
                        outerFlickable: flickable
                    }
                }
            }

            // Footer
            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Добавить")
                clickable: cardFront.valid && cardFront.text !== "" &&
                           cardBack.valid && cardBack.text !== ""
                onClicked:  {
                    createClicked(root.deckId,
                                  cardFront.text,
                                  cardBack.text)
                    root.close()
                }
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.topMargin: 4
                text: qsTr("Закрыть")
                onClicked: root.close()
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
    function open(deckId) {
        root.visible = true
        root.deckId = deckId
    }

    function close() {
        root.visible = false
        cardFront.text = ""
        cardBack.text = ""
        root.deckId = 0
        Qt.inputMethod.hide()
    }
}
