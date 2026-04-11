import QtQuick
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property string windowTitle: qsTr("Настройки - Revise")
    property bool openedAsWindow: false

    Revise.Background {
        id: background
        anchors.fill: parent
    }

    Revise.BlurOverlay {
        id: blurOverlay
        anchors.fill: parent
        sourceItem: background
        blur: 1.4
    }

    Revise.Toolbar {
        id: toolbar
        height: parent.height
    }
}
