// Card preview page

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import Revise as Revise

Item {
    id: root

    property var pageParams: null // Revise.Card expected

    property string front : pageParams ? pageParams.card.front : string()
    property string back : pageParams ? pageParams.card.back : string()
    property bool flipped: false

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    QQC.BusyIndicator {
        anchors.centerIn: parent
        width: parent.width * 0.35
        height: width
        running: htmlCard.loading
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8
        visible: !htmlCard.loading

        Revise.HtmlView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: root.flipped ? (countLines(root.back) > 2 ? verticalCenterText(root.back) : centerHtml(root.back))
                               : centerHtml(root.front)
        }

        Revise.Button {
            Layout.fillWidth: true
            text: qsTr("Перевернуть")
            onClicked: root.flip()
        }
    }

    function flip() {
        root.flipped = !root.flipped
    }
}
