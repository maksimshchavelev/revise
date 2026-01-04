// Card creation page
import QtQuick
import QtQuick.Layouts
import "../theme"
import "../controls"

Item {
    id: root

    property int deckId: 0

    signal createClicked(int deckId, string front, string back)
    signal previewClicked(string front, string back)
    signal backClicked

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        AppText {
            text: qsTr("Текст спереди")
        }

        ValidatedTextField {
            id: cardFront
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            placeholderText: qsTr("Текст спереди")
            onTextChanged: {
                valid = !deckService.is_card_exists(root.deckId, cardFront.text)
            }
        }

        AppText {
            text: qsTr("Текст сзади")
        }

        ValidatedMultilineTextField {
            id: cardBack
            Layout.fillWidth: true
            Layout.fillHeight: true
            placeholderText: qsTr("Текст сзади")
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
