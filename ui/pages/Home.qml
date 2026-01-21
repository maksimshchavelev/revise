// Home page
import QtQuick
import QtQuick.Layouts
import "../views/decks"
import "../controls"
import "../theme"

Item {
    id: root

    signal deckClicked(int deckId, bool repeatableToday)

    ListView {
        model: decksModel
        spacing: 10
        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: parent
        anchors.margins: 5
        clip: true
        
        delegate: Item {
            width: ListView.view.width
            height: childrenRect.height

            DeckItem {
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
