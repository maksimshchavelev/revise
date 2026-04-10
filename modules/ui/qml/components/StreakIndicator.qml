import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property real streak: 0
    property bool updatedToday: false

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    RowLayout {
        id: layout
        spacing: 2

        StatefulIndicator {
            showBackground: false
            active: root.updatedToday
            activeIcon: "qrc:/res/img/streak_updated.svg"
            inactiveIcon: "qrc:/res/img/streak.svg"
            width: 36
            height: 36
        }

        Revise.Text {
            text: streak
            font.pointSize: Revise.Theme.fontSizeBig
            font.bold: true
            color: root.updatedToday ? Revise.Theme.streakUpdatedTextColor : Revise.Theme.textColorDark
        }
    }
}
