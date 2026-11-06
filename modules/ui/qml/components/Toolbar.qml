// Toolbar.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype Sidebar
    \brief The main navigation sidebar of the application.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    Sidebar provides top-level navigation between the main pages of the
    application, shows the current streak, and displays the application
    version. It is intended for the desktop layout, where navigation lives
    in a persistent panel alongside the content.

    The active menu item is derived from
    \l {Revise.Router::currentPage}{Revise.Router.currentPage}, so the
    sidebar stays in sync with navigation regardless of where the page
    change originated. Clicking a menu item navigates through
    \l {Revise.Router::navigate}{Revise.Router.navigate}.

    The streak display is bound to \l Revise.StreakService and requires no
    additional wiring from the consumer.

    The component has no public API of its own; its behavior is driven by
    \l Revise.Router, \l Revise.StreakService, and \l Revise.BuildInfo.

    \section1 Example Usage

    \qml
    Revise.Sidebar {
        Layout.fillHeight: true
    }
    \endqml

    \sa Revise.MenuButton, Revise.StreakIndicator, Revise.Router
*/
Item {
    id: root

    width: 150
    height: layout.implicitHeight

    layer.effect: Revise.Shadow{}

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
            selected: Revise.Router.currentPage.name === "settings"
            Layout.fillWidth: true
            onClicked: Revise.Router.navigate("settings", {})
        }

        Revise.VerticalSpacer {}

        Revise.Text {
            color: Revise.Theme.textColorDark
            font.pixelSize: Revise.Theme.fontSizeSmall
            text: {
                if (Revise.BuildInfo.isBetaBuild)
                    return qsTr(`Revise ${Revise.BuildInfo.version}b`)
                return qsTr(`Revise ${Revise.BuildInfo.version}`)
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
