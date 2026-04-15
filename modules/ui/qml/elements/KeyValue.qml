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

Row {
    id: root
    spacing: 6

    property alias key: key
    property alias value: value

    property int keyWidth: 100

    Revise.Text {
        id: key
        text: "key"
        color: Revise.Theme.textColorDark
        width: root.keyWidth
    }

    Revise.Text {
        id: value
        text: "value"
        Layout.alignment: Qt.AlignRight
        Layout.fillWidth: true
        horizontalAlignment: Revise.Text.AlignRight
        width: root.width - key.width - root.spacing
    }
}
