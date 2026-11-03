// KebabMenuButton.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype KebabMenuButton
    \brief A compact button for use inside a kebab menu.
    \inqmlmodule Revise
    \inherits Revise.Button
    \since 1.0

    KebabMenuButton is a specialized \l Revise.Button intended for menu
    items in a kebab (overflow) menu. It fills the available width,
    uses a reduced height, and renders a small text label with a
    transparent background that highlights on hover.

    The component inherits all public properties and signals of
    \l Revise.Button, including \l {Revise.Button::text}{text},
    \l {Revise.Button::clicked}{clicked}, and
    \l {Revise.Button::color}{color}. It does not introduce any
    additional public API.

    \section1 Example Usage

    \qml
    KebabMenuButton {
        text: qsTr("Delete")
        onClicked: deleteItem()
    }
    \endqml

    \sa Revise.Button, Revise.Menu
*/
Revise.Button {
    id: root

    Layout.fillWidth: true
    Layout.preferredHeight: 25

    backgroundItem.opacity: 0.05

    backgroundItem.color: {
        if (root.hovered)
            return "white"
        return "transparent"
    }

    contentItem: Revise.Text {
        text: root.text
        color: root.color
        font.pixelSize: Revise.Theme.fontSizeSmall
        opacity: root.clickable ? 1.0 : 0.5
    }
}
