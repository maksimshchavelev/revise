import QtQuick
import Revise as Revise

Revise.Button {
    id: root

    property bool selected: false

    background.color: Qt.lighter(Revise.Theme.backgroundLight)
    background.opacity: root.selected ? 0.3 : 0.0
    radius: 10
}
