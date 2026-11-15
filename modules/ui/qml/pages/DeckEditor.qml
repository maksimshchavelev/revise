// DeckEditor.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype DeckEditor
    \brief A responsive wrapper for the deck editor page.
    \inqmlmodule Revise
    \inherits Loader
    \since 1.0

    DeckEditor dynamically loads the appropriate interface for editing a deck
    based on the platform or user preference. It uses the
    \l {Revise.SettingsService::useMobileUi}{Revise.SettingsService.useMobileUi} property
    to switch between \l Revise.DesktopDeckEditor and \l Revise.MobileDeckEditor.

    The component serves as a unified entry point for the router and exposes the
    necessary lifecycle methods (\l onEnter and \l onExit) and properties
    (such as \l pageParams and \l windowTitle), proxying them to the currently
    loaded child component.

    \section1 Example Usage

    \qml
    Revise.Router.navigate("deckEditor", { "deckId": 42 })
    \endqml
*/
Loader {
    id: root

    /*!
        \qmlproperty var DeckEditor::pageParams
        \brief The parameters passed to the page during navigation.

        Typically contains the \c deckId key.
    */
    property var pageParams: null

    /*!
        \qmlproperty bool DeckEditor::openedAsWindow
        \brief Whether the page is opened as a separate window.

        The default value is \c false.
    */
    property bool openedAsWindow: false

    /*!
        \qmlproperty string DeckEditor::windowTitle
        \brief The title displayed in the window header.

        This property is read from the currently loaded child component.
    */
    property string windowTitle: root.item ? root.item.windowTitle : qsTr("Редактирование колоды - Revise")

    sourceComponent: Revise.SettingsService.useMobileUi ? mobileComponent : desktopComponent

    Component {
        id: desktopComponent
        Revise.DesktopDeckEditor { }
    }

    Component {
        id: mobileComponent
        Revise.MobileDeckEditor { }
    }

    Binding {
        target: root.item
        property: "pageParams"
        value: root.pageParams
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "openedAsWindow"
        value: root.openedAsWindow
        when: root.item !== null
    }

    /*!
        \qmlmethod void DeckEditor::onEnter()
        \brief Triggers the onEnter lifecycle method of the loaded child component.
    */
    function onEnter() {
        if (root.item)
            root.item.onEnter()
    }

    /*!
        \qmlmethod void DeckEditor::onExit()
        \brief Triggers the onExit lifecycle method of the loaded child component.
    */
    function onExit() {
        if (root.item)
            root.item.onExit()
    }
}
