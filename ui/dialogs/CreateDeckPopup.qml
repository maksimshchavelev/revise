// Deck creation popup
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

    signal createClicked(string name,
                         string description,
                         int timeLimit,
                         int newLimit,
                         int consolidateLimit,
                         int incorrectLimit)

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
        opacity: 0.25

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
        height: parent.height * 0.65
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
                    spacing: 8

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
                            top: 3600 // 1 hour
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
                            top: 500 // 1 hour
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
                            top: 500 // 1 hour
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
                            top: 500 // 1 hour
                        }
                    }
                }
            }

            // Footer
            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: parent.width * 0.85
                Layout.topMargin: 4
                text: qsTr("Создать")
                clickable: deckName.valid && deckName.text !== "" &&
                           (deckDescription.valid || deckDescription.text === "") &&
                           (!limitTime.checked || (deckTimeLimit.valid && deckTimeLimit.text !== "")) &&
                           deckNewLimit.valid && deckNewLimit.text !== "" &&
                           deckConsolidateLimit.valid && deckConsolidateLimit.text !== "" &&
                           deckIncorrectLimit.valid && deckIncorrectLimit.text !== ""
                onClicked:  {
                    createClicked(deckName.text,
                                  deckDescription.text,
                                  limitTime.checked ? parseInt(deckTimeLimit.text) : 0,
                                  parseInt(deckNewLimit.text),
                                  parseInt(deckConsolidateLimit.text),
                                  parseInt(deckIncorrectLimit.text))
                    root.close()
                }
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: parent.width * 0.85
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
    function open() {
        root.visible = true
    }

    function close() {
        root.visible = false
        deckName.text = ""
        deckDescription.text = ""
        deckTimeLimit.text = ""
        deckNewLimit.text = ""
        deckConsolidateLimit.text = ""
        deckIncorrectLimit.text = ""
        Qt.inputMethod.hide()
    }
}
