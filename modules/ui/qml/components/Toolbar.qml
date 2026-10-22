// The sidebar displayed on the left (on the desktop) with the navigation menu and the streak display

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

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
        color: Revise.Theme.surfaceColor
        anchors.fill: parent
    }

    ColumnLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: 6
        anchors.topMargin: 12
        spacing: 12

        Revise.StreakIndicator {
            streak: Revise.StreakService.streak
            updatedToday: Revise.StreakService.updatedToday
            Layout.alignment: Qt.AlignHCenter
        }

        Revise.MenuButton {
            text: qsTr("Колоды")
            selected: Revise.Router.currentPage.name === "home"
            Layout.fillWidth: true
            onClicked: Revise.Router.navigate("home", {})
        }

        Revise.MenuButton {
            text: qsTr("Настройки")
            selected: Revise.Router.currentPage.name === "Revise.Settings"
            Layout.fillWidth: true
            onClicked: Revise.Router.navigate("Revise.Settings", {})
        }

        Revise.VerticalSpacer {}

        Revise.Text {
            color: Revise.Theme.textColorDark
            font.pixelSize: Revise.Theme.fontSizeSmall
            text: qsTr(`Revise ${Revise.BuildInfo.version}`)
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
