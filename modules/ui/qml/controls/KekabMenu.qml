import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    default property alias content: layout.data
    property alias opened: popup.opened

    property int windowWidth: Window.window ? Window.window.width : 0
    property int windowHeight: Window.window ? Window.window.height : 0

    signal closed()

    Popup {
        id: popup

        closePolicy: Popup.CloseOnPressOutsideParent
        modal: false
        focus: true
        padding: 5
        scale: opened ? 1.0 : 0.7
        opacity: opened ? 1.0 : 0

        background: Rectangle {
            radius: 10
            color: Revise.Theme.background
        }

        ColumnLayout {
            id: layout
            spacing: 4
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 75
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 60
            }
        }
    }

    function clampPosition() {
        let mapped = root.mapToItem(null, 0, 0)

        let menuX = mapped.x + popup.implicitWidth + popup.x
        let menuY = mapped.y + popup.implicitHeight + popup.y

        if (menuX > windowWidth) {
            popup.x -= menuX - windowWidth
        }

        if (menuY > windowHeight) {
            popup.y -= menuY - windowHeight
        }
    }

    function open() {
        popup.open()
    }

    function close() {
        popup.close()
        root.closed()
    }
}
