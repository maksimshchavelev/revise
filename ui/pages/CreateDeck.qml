
// Card creation page
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

    property bool modal: true
    property real maxHeightRatio: 0.85
    property int deckId: 0

    signal createClicked(int deckId, string front, string back)
    signal previewClicked(string front, string back)
    signal backClicked

    // Main column layout
    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 8
        anchors.topMargin: 2
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

                Item {
                    Layout.fillWidth: true
                }

                // Card front input
                ValidatedTextField {
                    id: cardFront
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    placeholderText: qsTr("Текст спереди")
                    onTextChanged: {
                        valid = !deckService.is_card_exists(root.deckId,
                                                            cardFront.text)
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
        RowLayout {
            spacing: 5
            Layout.fillWidth: true

            property real maxButtonWidth: Math.max(btnAdd.implicitWidth,
                                                   btnPreview.implicitWidth)

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: btnAdd.implicitHeight

                Button {
                    id: btnAdd
                    anchors.fill: parent
                    text: qsTr("Добавить")
                    clickable: cardFront.valid && cardFront.text.trim() !== ""
                               && cardBack.valid && cardBack.text.trim() !== ""
                    onClicked: {
                        createClicked(root.deckId, cardFront.text,
                                      cardBack.text)
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: btnAdd.implicitHeight

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
            onClicked: root.backClicked()
        }
    }
}
