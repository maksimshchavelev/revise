import QtQuick
import Revise as Revise

Revise.Button {
    id: root

    property bool selected: false

    backgroundItem.color: Qt.lighter(Revise.Theme.backgroundLight)
    backgroundItem.opacity: root.selected ? 0.3 : 0.0
    radius: 10
}
