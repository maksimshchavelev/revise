// Home page

import QtQuick
import QtQuick.Layouts
import "../components"

Item {
    id: root

    signal deckClicked(int deckId, bool repeatableToday) // Emitted when the user clicks on the deck. Passes the deck ID.

    ListView {
        model: decksModel
        spacing: 10
        boundsBehavior: Flickable.StopAtBounds

        width: parent.width * 0.95
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10

        delegate: Item {
            width: ListView.view.width
            height: childrenRect.height

            Deck {
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                deckName: model.name
                isTimeLimited: model.timeLimit > 0
                newCards: model.newCards
                consolidateCards: model.consolidateCards
                incorrectCards: model.incorrectCards
                deckId: model.deckId
                repeatableToday: model.repeatableToday

                onClicked: {
                    root.deckClicked(deckId, repeatableToday)
                }
            }
        }
    }
}
