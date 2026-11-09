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
        color: Color.darker(Revise.Theme.backgroundColor, 3)
    }

    Revise.Text {
        text: root.text
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 10
        font.pixelSize: Revise.Theme.fontSizeSmall
    }
}
