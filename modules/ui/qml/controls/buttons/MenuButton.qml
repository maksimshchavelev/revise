// MenuButton.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype MenuButton
    \brief A navigation button that highlights itself when selected.
    \inqmlmodule Revise
    \inherits Revise.Button
    \since 1.0

    MenuButton is a \l Revise.Button that lays out an icon and a left-aligned
    text label through an internal \l Revise.IconText, and shows a translucent
    background when \l selected is \c true.

    \section1 Example Usage

    \qml
    Revise.MenuButton {
        text: qsTr("Settings")
        icon.source: "qrc:/res/img/settings.svg"
        selected: Revise.Router.currentPage.name === "settings"
        onClicked: Revise.Router.navigate("settings", {})
    }
    \endqml
*/
Revise.Button {
    id: root

    /*!
        \qmlproperty bool MenuButton::selected
        \brief Whether the button is in the selected state.

        When \c true, the background is drawn with an opacity of \c 0.3;
        when \c false, the background is fully transparent. The property is
        controlled by the consumer and is not toggled by the button itself.

        The default value is \c false.
    */
    property bool selected: false

    backgroundItem.color: Revise.Theme.surfaceContainerColor
    backgroundItem.opacity: root.selected ? 0.3 : 0.0

    contentItem: Revise.IconText {
        icon.source: root.icon.source
        label.text: root.text
        label.horizontalAlignment: Revise.Text.AlignLeft
    }
}
