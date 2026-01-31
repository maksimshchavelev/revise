import QtQuick
import QtQuick.Layouts
import Revise

Item {
    id: root
    width: parent.width
    height: 75

    property int currentIndex: 0
    default property alias content: row.data
    signal pageChanged(int current)

    Rectangle {
        anchors.fill: parent
        color: Theme.toolbarColor
        opacity: 0.4
    }

    RowLayout {
        id: row
        anchors.margins: 12
        anchors.fill: parent
        clip: true
    }

    Component.onCompleted: {
        for (let i = 0; i < row.children.length; ++i) {
            let btn = row.children[i]
            btn.index = i
            btn.active = (i === currentIndex)
            btn.clicked.connect(setCurrent)
        }
    }

    onCurrentIndexChanged: {
        for (let j = 0; j < row.children.length; ++j) {
            row.children[j].active = (j === currentIndex)
        }
    }

    function setCurrent(i) {
        currentIndex = i
        pageChanged(i)
    }

    DebugBounds {}
}
