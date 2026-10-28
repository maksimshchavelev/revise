// Flickable.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype Flickable
    \brief Provides a vertically scrollable area with a custom scroll bar.
    \inherits Flickable
    \inqmlmodule Revise
    \since 1.0

    This type provides a vertically scrollable area with content clipping
    and a custom vertical scroll bar.

    Scrolling is restricted to the vertical direction. The content cannot
    be flicked beyond the bounds of the flickable area.

    The vertical scroll bar is displayed when needed, depending on the
    content size and the visible area.

    \sa QtQuick::Flickable, Revise::Scrollbar
*/
Flickable {
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    flickableDirection: Flickable.VerticalFlick
    ScrollBar.vertical: Revise.Scrollbar {
        policy: Revise.Scrollbar.AsNeeded
    }
}
