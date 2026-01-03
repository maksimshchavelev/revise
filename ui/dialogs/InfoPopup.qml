// A pop-up notification that displays plain text

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import "../theme"
import "../controls"

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property string text
    property Item sourceItem

    ShaderEffectSource {
        id: snapshot
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: true
        recursive: true
        hideSource: false
        smooth: true
    }

    MultiEffect {
        anchors.fill: parent
        source: snapshot
        blurEnabled: true
        blur: 1
        blurMax: 32
        autoPaddingEnabled: false
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.25
    }

    Item {
        width: parent.width * 0.85
        implicitHeight: text.implicitHeight + 110
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.4
            radius: 20
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Item {
                Layout.fillHeight: true
            }

            AppText {
                id: text
                text: root.text
                color: Theme.textColor
                Layout.alignment: Qt.AlignCenter
                Layout.maximumWidth: parent.width * 0.85
            }

            Button {
                text: qsTr("Закрыть")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.margins: 10
                onClicked: root.close()
            }
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 100
        }
    }

    function open(text) {
        root.text = text
        root.visible = true
    }

    function close() {
        root.visible = false
    }
}
