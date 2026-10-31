// TabButton.qml

import QtQuick
import QtQuick.Templates as T
import Revise as Revise

/*!
    \qmltype TabButton
    \brief A tab button for use inside a \l TabBar.
    \inqmlmodule Revise
    \inherits T.TabButton
    \since 1.0

    TabButton is a lightweight tab control designed to be used as a child of
    \l TabBar. It displays a text label and provides visual feedback for the
    checked state, including a colored background and a bottom indicator.

    The component inherits the standard \l T.TabButton API, including
    \l {T.TabButton::text}{text}, \l {T.TabButton::checked}{checked},
    \l {T.TabButton::checkable}{checkable}, and
    \l {T.TabButton::autoExclusive}{autoExclusive}. The \c checked property is
    managed automatically by the parent \l TabBar, so there is no need to
    toggle it manually.

    The default font is taken from \l Revise.Fonts.regular with the size
    defined by \l Revise.Theme.fontSizeDefault.

    \section1 Example Usage

    A basic tab bar with three tabs:

    \qml
    TabBar {
        TabButton { text: qsTr("General") }
        TabButton { text: qsTr("Advanced") }
        TabButton { text: qsTr("About") }
    }
    \endqml

    To enable horizontal scrolling when the total width of the tabs exceeds
    the width of the \l TabBar, set \c width to \c implicitWidth on each
    TabButton:

    \qml
    TabBar {
        width: parent.width

        TabButton { text: qsTr("General"); width: implicitWidth }
        TabButton { text: qsTr("Advanced"); width: implicitWidth }
        TabButton { text: qsTr("About"); width: implicitWidth }
    }
    \endqml

    \sa T.TabButton, TabBar
*/
T.TabButton {
    id: root

    font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    implicitWidth: txt.implicitWidth + 40
    implicitHeight: Math.max(35, txt.implicitHeight + 10)

    clip: true

    background: Rectangle {
        radius: 6
        color: {
            if (root.checked)
                return Color.lighter(Revise.Theme.surfaceContainerColor, 1.2)
            return Revise.Theme.surfaceContainerColor
        }

        Behavior on color {
            ColorAnimation { duration: 90 }
        }
    }

    contentItem: Revise.IconText {
        id: txt
        label.text: root.text
        label.font: root.font
        label.horizontalAlignment: Text.AlignHCenter
        label.verticalAlignment: Text.AlignVCenter
    }

    indicator: Rectangle {
        width: root.width
        height: 4
        radius: 2
        anchors.bottom: root.bottom
        visible: root.checked
        color: Revise.Theme.primaryColor

        Behavior on visible {
            NumberAnimation {
                duration: 90
            }
        }
    }

    HoverHandler {
        cursorShape: root.hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
