// Statistics page

import QtQuick
import QtQuick.Layouts
import "../components"

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        CheckupableOption {
            text: "Check me"
            Layout.fillWidth: true
        }
    }
}
