// IconText.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype IconText
    \inqmlmodule Revise
    \brief A layout component that displays an icon on the left and a responsive text label on the right.

    The IconText component pairs an icon with a text label. It safely supports
    multiline text wrapping and automatically scales its total height based on
    the tallest element inside.

    Example usage:
    \code
    Revise.IconText {
        width: parent.width
        label.text: qsTr("This is a very long descriptive text that will automatically wrap into multiple lines if it exceeds the available width.")
        icon.source: "qrc:/res/img/info.svg"
    }
    \endcode
*/
Item {
    id: root

    /*!
        This property provides access to the internal Text element to configure text, color, or font options.
     */
    property alias label: textLabel

    /*!
        This property provides access to the internal Icon element to configure source, size, or custom states.
     */
    property alias icon: iconImage

    width: parent ? parent.width : 0
    height: Math.max(iconImage.height, textLabel.implicitHeight)

    Revise.Icon {
        id: iconImage

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        color: Color.lighter(Revise.Theme.primaryColor, 1.5)
    }

    Revise.Text {
        id: textLabel

        anchors.left: iconImage.right
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        wrapMode: Text.WordWrap
    }
}
