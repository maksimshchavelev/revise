// Text and checkbox for options

import QtQuick
import QtQuick.Layouts
import Revise as Revise

RowLayout {
    id: root

    property string text
    property alias font: txt.font
    property alias checked: checkbox.checked

    signal clicked(bool checked)

    spacing: 10
    Layout.fillWidth: true

    Revise.Text {
        id: txt
        Layout.fillWidth: true
        text: root.text
        wrapMode: Text.WordWrap
    }

    Revise.Checkbox {
        id: checkbox
    }
}
