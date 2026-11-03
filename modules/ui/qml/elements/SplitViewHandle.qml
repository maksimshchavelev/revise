// SplitViewHandle.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype SplitViewHandle
    \brief Provides a visual handle for resizing panels in a SplitView.
    \inqmlmodule Revise

    SplitViewHandle provides a rounded rectangular indicator for the
    handle of a SplitView. Its appearance changes while the handle
    is being dragged.

    The handle is designed for use as the \c handle delegate of a
    SplitView. Its dimensions are determined by the implicit size of
    the delegate and the available space provided by the SplitView.

    The indicator uses the surface color defined by Revise.Theme,
    with increased brightness while the handle is pressed.

    \sa QtQuick.Controls::SplitView
*/

Rectangle {
    id: root

    implicitWidth: 24
    implicitHeight: 12

    color: "transparent"

    Rectangle {
        anchors.centerIn: parent

        width: 4
        height: parent.height - 12
        radius: width / 2

        color: root.SplitHandle.pressed
            ? Qt.lighter(Revise.Theme.surfaceColor, 1.8)
            : Qt.lighter(Revise.Theme.surfaceColor, 1.4)
    }
}
