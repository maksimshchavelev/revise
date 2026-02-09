// Card preview page
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import Revise

Item {
    id: root

    // front of the card
    property string front
    // back of the card
    property string back
    // flipped?
    property bool flipped: false
    // Previous page
    property int previousPageIndex: 0

    signal backClicked(int previousPageIndex)

    Shortcut {
        sequence: "Escape"
        onActivated: exitClicked()
    }

    function exitClicked() {
        root.flipped = false
        root.backClicked(root.previousPageIndex)
    }

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

        HtmlView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: root.flipped ? (countLines(root.back) > 2 ? verticalCenterText(root.back) : centerHtml(root.back))
                               : centerHtml(root.front)
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Перевернуть")
            onClicked: root.flipped = !root.flipped
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Назад")
            onClicked: {
                root.flipped = false
                root.backClicked(root.previousPageIndex)
            }
        }
    }
}
