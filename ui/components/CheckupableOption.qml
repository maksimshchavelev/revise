// Text and checkbox.

import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    property bool checked: false
    property string text

    spacing: 10
    Layout.fillWidth: true

    AppText {
        Layout.fillWidth: true
        text: root.text
        wrapMode: Text.WordWrap
    }

    Checkbox {
        checked: root.checked
    }
}
