// The sidebar displayed on the left (on the desktop) with the navigation menu and the streak display

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    height: 60
    width: parent.width

    Rectangle {
        id: background
        color: Revise.Theme.backgroundColor
        anchors.fill: parent
    }

    RowLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: 6
        anchors.topMargin: 12
        spacing: 0

        Revise.StreakIndicator {
            streak: Revise.StreakService.streak
            updatedToday: Revise.StreakService.updatedToday
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
                    color: Revise.Router.currentPage.name === "home" ? Revise.Theme.red : Revise.Theme.textColorDark
                    onClicked: Revise.Router.navigate("home", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}

                Revise.IconButton {
                    source: "qrc:/res/img/Revise.Settings.svg"
                    size: 32
                    color: Revise.Router.currentPage.name === "Revise.Settings" ? Revise.Theme.red : Revise.Theme.textColorDark
                    onClicked: Revise.Router.navigate("Revise.Settings", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}
            }
        }
    }
}
