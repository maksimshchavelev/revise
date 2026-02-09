// A pop-up notification with question and two buttons (yes / no)

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import Revise

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property string text
    property Item sourceItem

    signal acceptClicked()
    signal declineClicked()

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
        width: Math.min(parent.width * 0.85, 480)
        implicitHeight: layout.implicitHeight + 25
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
            anchors.margins: 12
            spacing: 15

            Item { Layout.fillWidth: true }

            AppText {
                id: text
                text: root.text
                color: Theme.textColor
                Layout.alignment: Qt.AlignCenter
                Layout.maximumWidth: parent.width * 0.85
            }

            RowLayout {
                Layout.preferredWidth: parent.width * 0.85
                spacing: 5

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: btnYes.implicitHeight

                    Button {
                        id: btnYes
                        text: qsTr("Да")
                        anchors.fill: parent
                        onClicked: {
                            root.close()
                            root.acceptClicked()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: btnNo.implicitHeight

                    Button {
                        id: btnNo
                        text: qsTr("Нет")
                        anchors.fill: parent
                        onClicked: {
                            root.close()
                            root.declineClicked()
                        }
                    }
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

    function open(text) {
        root.text = text
        root.visible = true
    }

    function close() {
        root.visible = false
        root.text = ""
    }
}
