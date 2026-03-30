import QtQuick
import QtQuick.Controls
import Revise as Revise

Popup {
    id: root

    property string text
    property int maxWidth: 300

    closePolicy: Popup.NoAutoClose
    modal: false
    focus: true
    padding: 10

    background: Rectangle {
        radius: root.height * 0.35
        color: Revise.Theme.background
    }

    Revise.Text {
        text: root.text
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 10
        font.pointSize: Revise.Theme.fontSizeSmall
    }
}
