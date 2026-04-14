/**
 * A single setting row with a label on the left and a value on the right.
 *
 * This element represents one configurable option in a settings panel.
 * The left side typically shows a descriptive text (label), while the right side
 * displays the current value / status
 */

import QtQuick
import QtQuick.Layouts
import Revise as Revise

RowLayout {
    id: root
    spacing: 10

    property alias key: key
    property alias value: value

    property font font: Qt.font({
        family: Revise.Fonts.regular.name,
        pointSize: Revise.Theme.fontSizeDefault
    })

    property int keyWidth: 100

    Revise.Text {
        id: key
        text: "key"
        font: root.font
        color: Revise.Theme.textColorDark
        Layout.alignment: Qt.AlignLeft
        Layout.preferredWidth: root.keyWidth
    }

    Revise.Text {
        id: value
        text: "value"
        font: root.font
        Layout.alignment: Qt.AlignRight
        Layout.fillWidth: true
    }
}
