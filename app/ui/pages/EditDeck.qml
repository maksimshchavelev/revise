// Edit deck
import QtQuick
import QtQuick.Layouts
import Revise

Item {
    id: root

    property int deckId: 0
    signal backClicked()

    Shortcut {
        sequence: "Esc"
        onActivated: {
            console.log("ESCAPE!")
            exitClicked()
        }
    }

    function exitClicked() {
        if (pages.children[pages.currentIndex] && typeof pages.children[pages.currentIndex].exitClicked === 'function') {
            pages.children[pages.currentIndex].exitClicked()
        } else {
            console.warn("Failed to check pages.children[pages.currentIndex] && typeof pages.children[pages.currentIndex].exitClicked === 'function'")
        }
    }

    StackLayout {
        id: pages
        anchors.fill: parent

        DeckOptions {
            deckId: root.deckId
            onAddCardClicked: {
                deckId = root.deckId
                pages.currentIndex = 1
            }
            onUpdateClicked: function (deckId, name, description, timeLimit, newLimit, consolidateLimit, incorrectLimit) {
                deckService.update_deck(deckId, name, description, timeLimit, newLimit, consolidateLimit, incorrectLimit)
            }
            onCardClicked: function(cardId) {
                cardEventPopup.open(cardId)
            }
            onBackClicked: root.backClicked()
        }

        CreateCard {
            deckId: root.deckId
            onBackClicked: pages.currentIndex = 0
            onCreateClicked: {
                deckService.create_card(deckId, front, back)
            }
            onPreviewClicked: function(front, back) {
                Qt.inputMethod.hide()
                previewCard.front = front
                previewCard.back = back
                previewCard.previousPageIndex = 1
                pages.currentIndex = 2
            }
        }

        PreviewCard {
            id: previewCard
            onBackClicked: function(previousPageIndex) {
                Qt.inputMethod.hide()
                pages.currentIndex = previousPageIndex
            }
        }

        EditCard {
            id: editCard

            onPreviewClicked: {
                Qt.inputMethod.hide()
                previewCard.front = front
                previewCard.back = back
                previewCard.previousPageIndex = 3
                pages.currentIndex = 2
            }

            onUpdateClicked: function(id, front, back) {
                deckService.update_card(id, front, back)
            }

            onBackClicked: function() {
                Qt.inputMethod.hide()
                pages.currentIndex = 0
            }
        }
    }

    CardEventPopup {
        id: cardEventPopup

        onPreviewClicked: function(cardId) {
            Qt.inputMethod.hide()
            var card = deckService.get_card(cardId)
            previewCard.front = card.front
            previewCard.back = card.back
            previewCard.previousPageIndex = 0
            pages.currentIndex = 2
        }

        onRemoveClicked: function(cardId) {
            deckService.remove_card(cardId)
        }

        onEditClicked: function(cardId) {
            Qt.inputMethod.hide()
            editCard.card = deckService.get_card(cardId)
            pages.currentIndex = 3
        }
    }
}
