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
    opacity: root.visible ? 1 : 0
    z: 1000

    signal importAnkiSelected()
    signal createSelected()
    signal rejected()

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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.close()
            root.rejected()
        }
    }

    Item {
        id: widget
        width: Math.min(parent.width * 0.85, 480)
        implicitHeight: layout.implicitHeight
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.6
            radius: 20
            border.color: Theme.popupBorder
            border.width: 2
        }

        ColumnLayout {
            id: layout
            anchors.fill: parent
            spacing: 15

            Button {
                text: qsTr("Импортировать")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 20
                onClicked: {
                    root.close()
                    root.importAnkiSelected()
                }
            }

            Button {
                text: qsTr("Создать")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 20
                onClicked: {
                    root.close()
                    root.createSelected()
                }
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

    function open() {
        root.visible = true
    }

    function close() {
        root.visible = false
    }
}
