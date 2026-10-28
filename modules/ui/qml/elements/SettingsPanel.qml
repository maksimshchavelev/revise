// SettingsPanel.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype SettingsPanel
    \brief A panel with a configurable header and settings content.
    \inherits PanelColumn
    \inqmlmodule Revise

    SettingsPanel provides a structured container for displaying related
    settings. It includes a header consisting of an icon and a text label,
    followed by content items supplied by the user.

    The header property provides access to the header's icon, label, and
    other properties exposed by Revise.IconText.

    The header icon size and label font size are set to
    \c{Revise.Theme.fontSizeMedium}. The header label uses a bold font.

    \section1 Example Usage

    The following example creates a settings panel for configuring
    answer time limitations:

    \qml
    Revise.SettingsPanel {
        width: parent.width

        header.icon.source: "qrc:/res/img/restrictions.svg"
        header.label.text: qsTr("Limitations")

        Revise.SettingsRow {
            width: parent.width
            text: qsTr("Limit answer time")

            Revise.Switch {
                checked: root.deck ? root.deck.timeLimit > 0 : false
            }
        }
    }
    \endqml

    \sa PanelColumn, Revise::IconText, Revise::SettingsRow
*/
Revise.PanelColumn {
    id: root

    property alias header: header

    Revise.IconText {
        id: header
        icon.size: Revise.Theme.fontSizeMedium
        label.font.pixelSize: Revise.Theme.fontSizeMedium
        label.font.bold: true
    }
}
