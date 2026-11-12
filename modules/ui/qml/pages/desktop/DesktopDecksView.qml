// DesktopDecksView.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype DesktopDecksView
    \brief A grid view displaying a list of decks for desktop interfaces.
    \inqmlmodule Revise
    \inherits GridView
    \since 1.0

    DesktopDecksView renders a collection of decks using a \l GridView, arranging
    them in a grid layout suitable for desktop screens. The view uses a
    \l DelegateChooser to display either a standard deck card or a special
    "Add Deck" card at the beginning of the list.

    The component automatically connects the actions from the delegates
    (such as removing or exporting a deck) to its own signals, allowing the
    parent component to handle the logic.

    \section1 Example Usage

    \qml
    Revise.DesktopDecksView {
        anchors.fill: parent

        onRemoveDeckClicked: (deckId) => confirmRemoveDeck(deckId)
        onExportDeckClicked: (deckId) => openExportDialog(deckId)
        onImportDeckClicked: openImportDialog()
        onCreateDeckClicked: openCreateDeckDrawer()
    }
    \endqml
*/
GridView {
    id: root

    /*!
        \qmlsignal DesktopDecksView::removeDeckClicked(int deckId)
        \brief Emitted when the user requests to remove a specific deck.

        The \a deckId parameter contains the identifier of the deck to be removed.
    */
    signal removeDeckClicked(int deckId);

    /*!
        \qmlsignal DesktopDecksView::exportDeckClicked(int deckId)
        \brief Emitted when the user requests to export a specific deck.

        The \a deckId parameter contains the identifier of the deck to be exported.
    */
    signal exportDeckClicked(int deckId);

    /*!
        \qmlsignal DesktopDecksView::importDeckClicked()
        \brief Emitted when the user requests to import a deck.
    */
    signal importDeckClicked();

    /*!
        \qmlsignal DesktopDecksView::createDeckClicked()
        \brief Emitted when the user requests to create a new deck.
    */
    signal createDeckClicked();

    model: Revise.DecksModel
    boundsBehavior: Flickable.StopAtBounds
    clip: true

    cellWidth: 180
    cellHeight: 230
    rightMargin: 6

    ScrollBar.vertical: Revise.Scrollbar {
        visible: root.contentHeight > root.height
    }

    delegate: DelegateChooser {
        role: "isSpecial"

        DelegateChoice {
            roleValue: true

            delegate: Revise.AddDeckDelegate {
                onImportClicked: root.importDeckClicked();
                onCreateClicked: root.createDeckClicked();
            }
        }

        DelegateChoice {
            roleValue: false

            delegate: Revise.DeckDelegate {
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
