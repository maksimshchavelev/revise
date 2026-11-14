// MobileToolbar.qml
// Mobile implementation of Toolbar

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    height: 60
    width: parent.width

    Rectangle {
        id: background
        color: Revise.Theme.surfaceColor
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
                    size: 36
                    background.color: "transparent"
                    icon.source: "qrc:/res/img/home.svg"
                    icon.color: Revise.Router.currentPage.name === "home" ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                    onClicked: Revise.Router.navigate("home", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}

                Revise.IconButton {
                    size: 36
                    background.color: "transparent"
                    icon.source: "qrc:/res/img/settings.svg"
                    icon.color: Revise.Router.currentPage.name === "settings" ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                    onClicked: Revise.Router.navigate("settings", {})
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Revise.HorizontalSpacer {}
            }
        }
    }
}
