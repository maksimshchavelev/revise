// A pop-up notification that displays what to do with the card

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

    signal removeClicked(int cardId)
    signal editClicked(int cardId)
    signal previewClicked(int cardId)

    property Item sourceItem
    property int cardId: 0

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
        opacity: 0.4
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
        width: parent.width * 0.75
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
                text: qsTr("Превью")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 20
                onClicked: {
                    root.close()
                    root.previewClicked(root.cardId)
                }
            }

            Button {
                text: qsTr("Редактировать")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    root.close()
                    root.editClicked(root.cardId)
                }
            }

            Button {
                text: qsTr("Удалить")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 20
                onClicked: {
                    root.close()
                    root.removeClicked(root.cardId)
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

    function open(cardId) {
        Qt.inputMethod.hide()
        root.cardId = cardId
        root.visible = true
    }

    function close() {
        root.visible = false
    }
}
