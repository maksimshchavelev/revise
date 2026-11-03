// KebabMenu.qml

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype KebabMenu
    \brief A popup menu container with a vertical stack of items and automatic screen edge clamping.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    KebabMenu provides a lightweight popup that arranges its children
    vertically. It is intended for use as a kebab (overflow) menu, where
    each child is typically a \l Revise.KebabMenuButton. The popup opens
    with a short scale and opacity animation.

    The popup position is automatically adjusted when it would otherwise
    extend beyond the bounds of the application window. The window size
    is tracked through the \l windowWidth and \l windowHeight properties,
    which are bound to the enclosing \l Window. If no window is available,
    these properties fall back to \c 0 and clamping is skipped.

    \section1 Example Usage

    \qml
    Revise.KebabMenu {
        id: kebabMenu

        Revise.KebabMenuButton {
            text: qsTr("Preview")
            onClicked: {
                kebabMenu.close()
                // Some action
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Edit")
            onClicked: {
                kebabMenu.close()
                // Some action
            }
        }
    }
    \endqml

    \sa Revise.KebabMenuButton
*/
Item {
    id: root

    /*!
        \qmlproperty list<QtObject> KebabMenu::content
        \brief The default property holding the menu items.

        Children declared inside a KebabMenu are automatically added to
        the internal vertical layout and become part of the popup.
    */
    default property alias content: layout.data

    /*!
        \qmlproperty bool KebabMenu::opened
        \brief Whether the popup is currently open.

        This property is read-only and reflects the state of the internal
        popup. To change it, use \l open() or \l close().
    */
    property alias opened: popup.opened

    /*!
        \qmlproperty int KebabMenu::windowWidth
        \brief The width of the enclosing window, in pixels.

        Used by the automatic edge clamping logic. When no window is
        available, the value is \c 0 and horizontal clamping is skipped.
    */
    property int windowWidth: Window.window ? Window.window.width : 0

    /*!
        \qmlproperty int KebabMenu::windowHeight
        \brief The height of the enclosing window, in pixels.

        Used by the automatic edge clamping logic. When no window is
        available, the value is \c 0 and vertical clamping is skipped.
    */
    property int windowHeight: Window.window ? Window.window.height : 0

    /*!
        \qmlsignal KebabMenu::closed()
        \brief Emitted after the popup has been closed via \l close().

        This signal is not emitted when the popup closes by other means,
        such as clicking outside of it.
    */
    signal closed()

    /*!
        \qmlmethod void KebabMenu::open()
        \brief Opens the popup.

        The popup is shown with a short scale and opacity animation.
        After opening, its position is not re-clamped automatically; call
        \l open() again after moving the menu if clamping is required.
    */
    function open() {
        popup.open()
    }

    /*!
        \qmlmethod void KebabMenu::close()
        \brief Closes the popup and emits \l closed().

        The popup is hidden and the \l closed signal is emitted
        immediately.
    */
    function close() {
        popup.close()
        root.closed()
    }

    Popup {
        id: popup

        closePolicy: Popup.CloseOnPressOutsideParent
        modal: false
        focus: true
        padding: 5
        scale: opened ? 1.0 : 0.7
        opacity: opened ? 1.0 : 0

        background: Rectangle {
            radius: 10
            color: Revise.Theme.background
        }

        ColumnLayout {
            id: layout
            spacing: 4
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 75
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 60
            }
        }
    }

    function clampPosition() {
        let mapped = root.mapToItem(null, 0, 0)

        let menuX = mapped.x + popup.implicitWidth + popup.x
        let menuY = mapped.y + popup.implicitHeight + popup.y

        if (menuX > windowWidth) {
            popup.x -= menuX - windowWidth
        }

        if (menuY > windowHeight) {
            popup.y -= menuY - windowHeight
        }
    }
}
