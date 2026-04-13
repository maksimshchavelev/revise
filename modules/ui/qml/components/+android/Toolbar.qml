// The sidebar displayed on the left (on the desktop) with the navigation menu and the streak display

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property alias background: background

    height: 70
    width: parent.width

    Rectangle {
        id: background
        color: Revise.Theme.backgroundLight
        anchors.fill: parent
    }

    RowLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: 6
        anchors.topMargin: 12
        spacing: 0

        Revise.StreakIndicator {
            streak: streakService.streak
            updatedToday: streakService.updatedToday
            Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Revise.HorizontalSpacer {}

                Revise.IconButton {
                    source: "qrc:/res/img/home.svg"
                    size: 32
                    color: router.currentPage.name === "home" ? Revise.Theme.red : Revise.Theme.textColorDark
                    onClicked: router.navigate("home", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}

                Revise.IconButton {
                    source: "qrc:/res/img/settings.svg"
                    size: 32
                    color: router.currentPage.name === "settings" ? Revise.Theme.red : Revise.Theme.textColorDark
                    onClicked: router.navigate("settings", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}
            }
        }
    }
}
