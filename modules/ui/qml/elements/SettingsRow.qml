import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property alias text: label.text
    default property alias control: controlSlot.data

    implicitHeight: row.implicitHeight
    Layout.fillWidth: true

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 8

        Revise.Text {
            id: label
            Layout.preferredWidth: Revise.Theme.settingsLabelWidth
            Layout.alignment: Qt.AlignVCenter
            wrapMode: Text.WordWrap
        }

        ColumnLayout {
            id: controlSlot
            Layout.fillWidth: true
        }
    }
}