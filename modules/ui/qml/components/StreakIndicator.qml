// StreakIndicator.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype StreakIndicator
    \brief A visual indicator for the user's daily streak.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    StreakIndicator displays the current streak value alongside a corresponding icon.
    It visually distinguishes whether the streak has been updated today by changing
    both the icon and the text color.

    The component automatically sizes itself based on the internal layout.

    \section1 Example Usage

    \qml
    Revise.StreakIndicator {
        streak: Revise.StreakService.streak
        updatedToday: Revise.StreakService.updatedToday
    }
    \endqml
*/
Item {
    id: root

    /*!
        \qmlproperty real StreakIndicator::streak
        \brief The current streak value.

        The value is displayed next to the streak icon.

        The default value is \c 0.
    */
    property real streak: 0

    /*!
        \qmlproperty bool StreakIndicator::updatedToday
        \brief Whether the streak has been updated today.

        When \c true, a distinct "updated" icon is shown, and the text is colored
        with \l {Revise.Theme::streakUpdatedTextColor}{Revise.Theme.streakUpdatedTextColor}.
        When \c false, a default streak icon is shown, and the text uses
        \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor}.

        The default value is \c false.
    */
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
            width: 40
            height: 40
        }

        Revise.Text {
            text: streak
            font.pixelSize: Revise.Theme.fontSizeBig
            font.bold: true
            color: root.updatedToday ? Revise.Theme.streakUpdatedTextColor : Revise.Theme.secondaryForegroundColor
        }
    }
}
