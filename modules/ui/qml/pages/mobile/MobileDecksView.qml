// MobileDecksView.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype MobileDecksView
    \brief A mobile view displaying a vertical list of decks.
    \inqmlmodule Revise
    \inherits ListView
    \since 1.0

    MobileDecksView renders the list of decks using a \l ListView, where each delegate
    stretches to fill the available width. It uses a \l DelegateChooser to display
    either the "Add Deck" card or a standard deck card.

    \section1 Example Usage

    \qml
    Revise.MobileDecksView {
        anchors.fill: parent
        onRemoveDeckClicked: (deckId) => deleteDeck(deckId)
        onCreateDeckClicked: openEditor()
    }
    \endqml
*/
ListView {
    id: root

    /*!
        \qmlsignal MobileDecksView::removeDeckClicked(int deckId)
        \brief Emitted when the user requests to remove a deck.
    */
    signal removeDeckClicked(int deckId);

    /*!
        \qmlsignal MobileDecksView::exportDeckClicked(int deckId)
        \brief Emitted when the user requests to export a deck.
    */
    signal exportDeckClicked(int deckId);

    /*!
        \qmlsignal MobileDecksView::importDeckClicked()
        \brief Emitted when the user requests to import a deck.
    */
    signal importDeckClicked();

    /*!
        \qmlsignal MobileDecksView::createDeckClicked()
        \brief Emitted when the user requests to create a new deck.
    */
    signal createDeckClicked();

    model: Revise.DecksModel
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    spacing: 12

    ScrollBar.vertical: Revise.Scrollbar {
        visible: root.contentHeight > root.height
    }

    delegate: DelegateChooser {
        role: "isSpecial"

        DelegateChoice {
            roleValue: true

            delegate: Revise.AddDeckDelegate {
                width: ListView.view.width - 18
                onImportClicked: root.importDeckClicked();
                onCreateClicked: root.createDeckClicked();
            }
        }

        DelegateChoice {
            roleValue: false

            delegate: Revise.DeckDelegate {
                width: ListView.view.width - 18

                required property int newCards
                required property int reviewCards
                required property int incorrectCards
                required property int deckId
                required property int timeLimit
                required property string name
                required property string description
                required property bool repeatableToday

                deckNameProperty: name
                deckDescriptionProperty: description
                timeLimitProperty: timeLimit
                newCardsProperty: newCards
                reviewCardsProperty: reviewCards
                incorrectCardsProperty: incorrectCards
                deckIdProperty: deckId
                repeatableTodayProperty: repeatableToday

                onStudyClicked: {
                    Revise.StudyService.start(deckId)
                    Revise.Router.navigate("training", {})
                }

                onEditClicked: {
                    Revise.Router.navigate("deckEditor", {
                        "deckId": deckId
                    })
                }

                onRemoveClicked: root.removeDeckClicked(deckId)
                onExportClicked: root.exportDeckClicked(deckId)
            }
        }
    }
}
