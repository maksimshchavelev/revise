// CardEditorToolbar.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype CardEditorToolbar
    \brief A toolbar for the card editor providing navigation and save actions.
    \inqmlmodule Revise
    \inherits Rectangle
    \since 1.0

    CardEditorToolbar is a styled panel intended to be placed in a layout at the bottom or top of the card editor.
    It displays a hint about KaTeX usage, a button to navigate back using \l {Revise.Router::back}{Revise.Router.back()},
    and a primary action button to save the card.

    The text of the primary action button changes based on the \l isEditMode property.

    \section1 Example Usage

    \qml
    Revise.CardEditorToolbar {
        isEditMode: true
        onSaveClicked: saveCard()
    }
    \endqml
*/
Rectangle {
    id: root

    /*!
        \qmlsignal CardEditorToolbar::saveClicked()
        \brief Emitted when the primary action button is clicked.

        This signal should be handled to save the current state of the card being edited.
    */
        signal saveClicked

    /*!
        \qmlproperty bool CardEditorToolbar::isEditMode
        \brief Whether the editor is in edit mode.

        If \c true, the primary action button displays "Обновить" (Update).
        If \c false, it displays "Добавить" (Add).

        The default value is \c false.
    */
    property bool isEditMode: false

    Layout.fillWidth: true
    Layout.preferredHeight: 60
    color: Revise.Theme.surfaceColor
    radius: 8

    layer.effect: Revise.Shadow{}

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        spacing: 8

        // Use KaTeX tooltip
        Revise.IconText {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            icon.source: "qrc:/res/img/info.svg"
            icon.size: 20

            label.text: qsTr("Используйте KaTeX для формул")
            label.font.bold: true
            label.wrapMode: Revise.Text.NoWrap
            label.elide: Revise.Text.ElideRight
            label.maximumLineCount: 1
        }

        Revise.HorizontalSpacer {}

        Revise.Button {
            text: qsTr("Назад")
            onClicked: Revise.Router.back()
        }

        Revise.Button {
            text: root.isEditMode ? qsTr("Обновить") : qsTr("Добавить")
            backgroundItem.color: Revise.Theme.primaryColor
            onClicked: root.saveClicked()
        }
    }
}
