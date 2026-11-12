// DecksView.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype DecksView
    \brief A responsive view that switches between desktop and mobile deck layouts.
    \inqmlmodule Revise
    \inherits Loader
    \since 1.0

    DecksView acts as a dynamic wrapper that automatically loads the appropriate
    deck view based on the platform or user preference. It uses the
    \l {Revise.SettingsService::useMobileUi}{Revise.SettingsService.useMobileUi} property
    to determine whether to display \l DesktopDecksView or \l MobileDecksView.

    The component exposes a unified API by re-emitting the signals from the
    currently active child view. This allows the parent component to interact
    with the deck list without needing to know which specific layout is loaded.

    \section1 Example Usage

    \qml
    Revise.DecksView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        onRemoveDeckClicked: (deckId) => confirmRemoveDeck(deckId)
        onExportDeckClicked: (deckId) => openExportDialog(deckId)
        onImportDeckClicked: openImportDialog()
        onCreateDeckClicked: openCreateDeckDrawer()
    }
    \endqml
*/
Loader {
    id: root

    /*!
        \qmlsignal DecksView::removeDeckClicked(int deckId)
        \brief Emitted when the user requests to remove a specific deck.

        The \a deckId parameter contains the identifier of the deck to be removed.
    */
    signal removeDeckClicked(int deckId);

    /*!
        \qmlsignal DecksView::exportDeckClicked(int deckId)
        \brief Emitted when the user requests to export a specific deck.

        The \a deckId parameter contains the identifier of the deck to be exported.
    */
    signal exportDeckClicked(int deckId);

    /*!
        \qmlsignal DecksView::importDeckClicked()
        \brief Emitted when the user requests to import a deck.
    */
    signal importDeckClicked();

    /*!
        \qmlsignal DecksView::createDeckClicked()
        \brief Emitted when the user requests to create a new deck.
    */
    signal createDeckClicked();

    sourceComponent: Revise.SettingsService.useMobileUi ? mobileDecksView : desktopDecksView

    Component {
        id: desktopDecksView
        Revise.DesktopDecksView {}
    }

    Component {
        id: mobileDecksView
        Revise.MobileDecksView {}
    }

    Connections {
        target: root.item

        function onRemoveDeckClicked(deckId) {
            root.removeDeckClicked(deckId);
        }

        function onExportDeckClicked(deckId) {
            root.exportDeckClicked(deckId);
        }

        function onImportDeckClicked() {
            root.importDeckClicked();
        }

        function onCreateDeckClicked() {
            root.createDeckClicked();
        }
    }
}
