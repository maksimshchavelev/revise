// StatefulIndicator.qml

import QtQuick

/*!
    \qmltype StatefulIndicator
    \brief An indicator that switches between two icons based on its active state.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    StatefulIndicator displays an icon that changes depending on whether it is
    active or not. It is commonly used to represent states such as "selected",
    "enabled", or "completed". The component can optionally render a subtle
    translucent background behind the icon.

    By default, the component has a fixed square size of 96x96 pixels, but both
    dimensions can be overridden. The icon scales preserving its aspect ratio.

    \section1 Example Usage

    A basic active indicator:

    \qml
    Revise.StatefulIndicator {
        activeIcon: "qrc:/icons/active.svg"
        inactiveIcon: "qrc:/icons/inactive.svg"
        active: true
    }
    \endqml

    An indicator without a background and with custom margins:

    \qml
    Revise.StatefulIndicator {
        activeIcon: "qrc:/icons/star.svg"
        showBackground: false
        contentMargins: 10
    }
    \endqml
*/
Item {
    id: root

    /*!
        \qmlproperty string StatefulIndicator::activeIcon
        \brief The source URL of the icon displayed when the indicator is active.

        This property is expected to be set by the consumer.
    */
    property string activeIcon

    /*!
        \qmlproperty string StatefulIndicator::inactiveIcon
        \brief The source URL of the icon displayed when the indicator is inactive.

        If this property is not set, it defaults to the value of \l activeIcon,
        meaning the same icon will be used for both states.

        By default, this is bound to \l activeIcon.
    */
    property string inactiveIcon: activeIcon

    /*!
        \qmlproperty bool StatefulIndicator::active
        \brief Whether the indicator is in the active state.

        When \c true, \l activeIcon is displayed. When \c false, \l inactiveIcon
        is displayed.

        The default value is \c true.
    */
    property bool active: true

    /*!
        \qmlproperty bool StatefulIndicator::showBackground
        \brief Whether the translucent background is visible.

        The background is a rounded rectangle with a low opacity white color,
        providing contrast for the icon.

        The default value is \c true.
    */
    property bool showBackground: true

    /*!
        \qmlproperty real StatefulIndicator::contentMargins
        \brief The margin between the icon and the edges of the item, in pixels.

        This value is applied to all sides of the icon, effectively shrinking
        it inside the parent item.

        The default value is \c 5.
    */
    property real contentMargins: 5

    width: 96
    height: 96

    // Background
    Rectangle {
        visible: root.showBackground
        anchors.fill: parent
        opacity: 0.1
        color: "white"
        radius: height * 0.15
    }

    // Icon
    Image {
        source: root.active ? activeIcon : inactiveIcon
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
        anchors.margins: root.contentMargins
        anchors.centerIn: parent
        smooth: true
    }
}
