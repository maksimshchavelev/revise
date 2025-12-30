import QtQuick
import QtQuick.Layouts

StatefulIndicator {
    id: root

    property bool pressed: false
    property real index: -1 // The index used for the ToolBar. The ToolBar will know which button is pressed.
    property bool __isToolButton: true
    signal clicked(int index)

    scale: pressed ? 0.95 : 1
    contentMargins: 7

    Layout.preferredWidth: 48
    Layout.preferredHeight: 48
    Layout.alignment: Qt.AlignCenter

    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed = true
        onReleased: root.pressed = false
        onClicked: root.clicked(root.index)
    }

    Behavior on pressed {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 100
        }
    }
}
