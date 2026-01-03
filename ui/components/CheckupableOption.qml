// Text and checkbox.

import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    property bool checked: false
    property string text

    signal clicked(bool checked)

    spacing: 10
    Layout.fillWidth: true

    AppText {
        Layout.fillWidth: true
        text: root.text
        wrapMode: Text.WordWrap
    }

    Checkbox {
        id: checkbox
        checked: root.checked

        onCheckedChanged: {
            if (root.checked !== checkbox.checked) {
                root.checked = checkbox.checked
                root.clicked(root.checked)
            }
        }
    }

    function setChecked(value) {
        if (root.checked !== value) {
            root.checked = value
            checkbox.checked = value
        }
    }
}
