import QtQuick
import QtQuick.Layouts
import "../../theme"
import "../../components"
import "../../controls"

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
            activeIcon: "qrc:/res/img/streak_updated.png"
            inactiveIcon: "qrc:/res/img/streak.png"
            width: 50
            height: 50
        }

        AppText {
            text: streak
            font.pixelSize: Theme.textSizeBig
            font.bold: true
            color: root.updatedToday ? Theme.streakUpdatedTextColor : Theme.streakTextColor
        }
    }
}
