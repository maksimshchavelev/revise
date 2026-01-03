// A deck that will be displayed on the main page

import QtQuick
import QtQuick.Layouts
import "qrc:/ui/theme"

Item {
    id: root

    implicitHeight: text.implicitHeight + 20
    implicitWidth: 400
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignHCenter
    scale: root.pressed ? 0.98 : 1

    property string front
    property string back
    property int cardId: 0

    signal clicked(string cardId)

    property bool pressed: false

    Rectangle {
        anchors.fill: parent
        color: root.pressed ? "#111111" : "black"
        opacity: 0.4
        radius: 10
        clip: true

        Behavior on color {
            ColorAnimation {
                easing.type: Easing.OutBack
                duration: 100
            }
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 10
        clip: true

        AppText {
            id: text
            text: root.front
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed = true
        onReleased: root.pressed = false
        onClicked: root.clicked(root.deckName)
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 100
        }
    }
}

