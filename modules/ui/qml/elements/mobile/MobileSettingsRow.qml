import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property alias text: label.text
    default property alias control: controlSlot.data

    implicitHeight: row.implicitHeight
    Layout.fillWidth: true

    ColumnLayout {
        id: row
        anchors.fill: parent
        spacing: 8

        Revise.Text {
            id: label
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ColumnLayout {
            id: controlSlot
            Layout.fillWidth: true
        }
    }
}