// A deck that will be displayed on the main page

import QtQuick
import QtQuick.Layouts
import Revise

Item {
    id: root

    implicitHeight: row.implicitHeight + 20
    implicitWidth: 400
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignHCenter
    scale: root.pressed ? 0.98 : 1

    property real newCards: 0 // Cards that have not yet been studied
    property real consolidateCards: 0 // Cards that have already been studied and need to be reviewed
    property real incorrectCards: 0 // Cards that the user answered incorrectly during the last training session
    property real deckId: 0
    property string deckName: "" // Name of deck
    property bool isTimeLimited: false
    property bool repeatableToday: false

    signal clicked(string deckName)

    property bool pressed: false

    Rectangle {
        anchors.fill: parent
        color: root.pressed ? "#111111" : "black"
        opacity: 0.4
        radius: 15
        clip: true

        Behavior on color {
            ColorAnimation {
                easing.type: Easing.OutBack
                duration: 100
            }
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            id: row
            anchors.fill: parent
            spacing: 20

            RowLayout {
                spacing: 10

                StatefulIndicator {
                    visible: root.isTimeLimited
                    activeIcon: "qrc:/res/img/clocks.png"
                    showBackground: false
                    width: 20
                    height: 20
                    contentMargins: 0
                }

                AppText {
                    id: text
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    color: Theme.deckTextColor
                    text: root.deckName
                    font.bold: true
                }
            }

            // Statistics
            RowLayout {
                spacing: 5
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                AppText {
                    color: root.newCards == 0 ? Theme.deckTextColor : Theme.deckNewCardsTextColor
                    text: root.newCards
                    font.bold: true
                }

                AppText {
                    color: root.consolidateCards == 0 ? Theme.deckTextColor : Theme.deckConsolidateCardsTextColor
                    text: root.consolidateCards
                    font.bold: true
                }

                AppText {
                    color: root.incorrectCards == 0 ? Theme.deckTextColor : Theme.deckIncorrectCardsTextColor
                    text: root.incorrectCards
                    font.bold: true
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed = true
        onReleased: root.pressed = false
        onClicked: root.clicked(root.deckName)
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 100
        }
    }

    DebugBounds {}
}
