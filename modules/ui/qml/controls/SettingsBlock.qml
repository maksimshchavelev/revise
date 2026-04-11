import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property alias header: header
    default property alias content: content.data

    implicitWidth: layout.implicitWidth + layout.anchors.margins * 2
    implicitHeight: layout.implicitHeight + layout.anchors.margins * 2

    Layout.fillWidth: true

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 5
        verticalOffset: 5
        radius: 8
        samples: 32
        color: "#80000000"
    }

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 10
        color: Revise.Theme.backgroundLight
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 12
        anchors.margins: 12

        Revise.Text {
            id: header
            font.pointSize: Revise.Theme.fontSizeMedium
            font.bold: true
            text: qsTr("Настройка")
            Layout.fillWidth: true
        }

        ColumnLayout {
            id: content
            spacing: 8
        }
    }
}
