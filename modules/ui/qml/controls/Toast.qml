// Toast message
import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Rectangle {
    id: root

    property string header
    property string text
    property int duration: 4000 // ms

    property alias icon: icon.source

    signal opened
    signal closed

    width: 280
    height: rowLayout.implicitHeight + 20
    radius: 10
    visible: false
    color: Revise.Theme.toastSuccess

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 5
        verticalOffset: 5
        radius: 8
        samples: 32
        color: "#80000000"
    }

    // Block clicks through toasts
    MouseArea {
        anchors.fill: parent
    }

    RowLayout {
        id: rowLayout
        spacing: 10

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: 10
        }

        Item {
            Layout.preferredWidth: 36
            Layout.preferredHeight: 36

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.3
                radius: width * 0.3
            }

            Revise.Icon {
                id: icon
                anchors.fill: parent
                anchors.margins: 4
                source: "qrc:/res/img/success.svg"
            }
        }

        ColumnLayout {
            id: columnLayout

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4

            Revise.Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.header
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                maximumLineCount: 2
                color: "black"
                opacity: 0.75
                wrapMode: Text.WordWrap
            }

            Revise.Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.text
                font.pixelSize: Revise.Theme.fontSizeSmall
                horizontalAlignment: Text.AlignLeft
                color: "black"
                maximumLineCount: 10
                opacity: 0.75
                wrapMode: Text.WordWrap
            }
        }

        Revise.IconButton {
            id: closeButton
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            source: "qrc:/res/img/close.svg"
            onClicked: root.close()
        }

        Item {}
    }

    property NumberAnimation openAnim: NumberAnimation {
        target: root
        property: "x"
        duration: 300
        easing.type: Easing.OutCubic
    }

    property NumberAnimation closeAnim: NumberAnimation {
        target: root
        property: "x"
        duration: 300
        easing.type: Easing.InCubic
        onFinished: {
            root.visible = false
            root.closed()
        }
    }

    Timer {
        id: closeTimer
        interval: root.duration
        running: false
        onTriggered: root.close()
    }

    function open() {
        if (!parent)
            return

        root.visible = true
        root.x = parent.width

        openAnim.to = parent.width - width - 20
        openAnim.start()

        closeTimer.start()
        opened()
    }

    function close() {
        if (!parent)
            return

        closeAnim.to = parent.width
        closeAnim.start()
        closeTimer.stop()
    }
}
