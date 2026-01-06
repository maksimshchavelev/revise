// A modal dialog box that allows you to choose what to do with the deck. The user can modify it, start training, or
// delete it.

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import "../../theme"
import "../../controls"

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property Item sourceItem

    // Deck options
    property real deckId: 0
    property bool repeatableToday: false

    signal trainClicked(int deckId, bool repeatableToday)
    signal editClicked(int deckId)
    signal removeClicked(int deckId)

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

        MouseArea {
            anchors.fill: parent
            onClicked: root.close()
        }
    }

    Item {
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
                text: qsTr("Тренировать")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.topMargin: 15
                onClicked: {
                    trainClicked(root.deckId, repeatableToday)
                    root.close()
                }
            }

            Button {
                text: qsTr("Редактировать")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                onClicked: {
                    root.editClicked(root.deckId)
                    root.close()
                }
            }

            Button {
                text: qsTr("Удалить")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.bottomMargin: 15
                onClicked: {
                    root.removeClicked(root.deckId)
                    root.close()
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

    function open(deckId, repeatableToday) {
        root.deckId = deckId
        root.repeatableToday = repeatableToday
        root.visible = true
    }

    function close() {
        root.visible = false
    }
}

