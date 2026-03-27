# Header with streak and add deck button

import QtQuick
import Revise as Revise

Item {
    id: root

    signal addDeckClicked

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.toolbarColor
        opacity: 0.4
    }

    Revise.StreakIndicator {
        streak: streakService.streak
        updatedToday: streakService.updatedToday
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.margins: 15
    }

    Revise.ToolButton {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.margins: 15
        activeIcon: "qrc:/res/img/add_deck.png"
        width: 36
        height: 36
        showBackground: false
        contentMargins: 0

        onClicked: root.addDeckClicked()
    }
}
