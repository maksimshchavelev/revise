// The sidebar displayed on the left (on the desktop) with the navigation menu and the streak display

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property alias background: background

    width: 150
    height: layout.implicitHeight

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 4
        verticalOffset: 4
        radius: 8
        samples: 32
        color: "#80000000"
    }

    Rectangle {
        id: background
        color: Revise.Theme.backgroundLight
        anchors.fill: parent
    }

    ColumnLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: 6
        anchors.topMargin: 12
        spacing: 12

        Revise.StreakIndicator {
            streak: streakService.streak
            updatedToday: streakService.updatedToday
            Layout.alignment: Qt.AlignHCenter
        }

        Revise.MenuButton {
            text: qsTr("Колоды")
            selected: router.currentPage.name === "home"
            Layout.fillWidth: true
            onClicked: router.navigate("home", {})
        }

        Revise.MenuButton {
            text: qsTr("Настройки")
            selected: router.currentPage.name === "settings"
            Layout.fillWidth: true
            onClicked: router.navigate("settings", {})
        }

        Revise.VerticalSpacer {}

        Revise.Text {
            color: Revise.Theme.textColorDark
            font.pixelSize: Revise.Theme.fontSizeSmall
            text: qsTr("Revise 1.3.0")
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
