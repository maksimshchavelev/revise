// Scrollbar.qml

import QtQuick
import QtQuick.Controls.Basic
import Revise as Revise

/*!
    \qmltype Scrollbar
    \brief A vertical scroll bar styled for the Revise design system.
    \inqmlmodule Revise
    \inherits ScrollBar
    \since 1.0

    Scrollbar is a styled scroll bar that provides a rounded background track
    and a rounded slider. The background uses
    \l {Revise.Theme::surfaceColor}{Revise.Theme.surfaceColor}, while the
    slider uses a lighter variant of the same color and brightens further
    when the pointer hovers over it.

    The control is configured with \c {policy: ScrollBar.AsNeeded},
    \c {interactive: true}, and \c {minimumSize: 0.1} by default, so it
    appears only when the content overflows and remains draggable when it
    does. These inherited properties can still be overridden by consumers.

    The two visual parts of the scroll bar are exposed as aliases, allowing
    consumers to adjust their colors or other properties without replacing
    the components themselves.

    \section1 Example Usage

    A scroll bar attached to a \l ListView:

    \qml
    ListView {
        anchors.fill: parent
        model: 100
        delegate: Item { height: 40; width: ListView.view.width }

        ScrollBar.vertical: Revise.Scrollbar {
            backgroundItem.color: Revise.Theme.surfaceContainerColor
            sliderItem.color: Revise.Theme.primaryColor
        }
    }
    \endqml

    \sa ScrollBar
*/
ScrollBar {
    policy: ScrollBar.AsNeeded
    interactive: true
    minimumSize: 0.1

    /*!
        \qmlproperty Rectangle Scrollbar::backgroundItem
        \brief The background track of the scroll bar.

        This property provides access to the internal \l Rectangle that
        renders the track behind the slider. It can be used to customize
        the track color, radius, or other visual properties without
        replacing the background component.
    */
    property alias backgroundItem: backgroundItem

    /*!
        \qmlproperty Rectangle Scrollbar::sliderItem
        \brief The draggable slider of the scroll bar.

        This property provides access to the internal \l Rectangle that
        renders the slider. The slider color changes on hover: it uses a
        lighter variant of
        \l {Revise.Theme::surfaceColor}{Revise.Theme.surfaceColor} normally
        and a brighter variant when the pointer hovers over it. The alias
        allows consumers to override the color or other visual properties
        directly.
    */
    property alias sliderItem: sliderItem

    background: Rectangle {
        id: backgroundItem
        color: Revise.Theme.surfaceColor
        radius: width / 2
    }

    contentItem: Rectangle {
        id: sliderItem

        implicitWidth: 8
        radius: width / 2
        color: hoverHandler.hovered ? Color.lighter(Revise.Theme.surfaceColor, 1.8) : Color.lighter(Revise.Theme.surfaceColor, 1.4)

        HoverHandler {
            id: hoverHandler
        }
    }
}
