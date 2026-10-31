// Button.qml

import QtQuick
import QtQuick.Templates as T
import Revise as Revise

/*!
    \qmltype Button
    \brief A button with optional text, icon, and loading indicator.
    \inqmlmodule Revise
    \inherits T.Button

    Button provides an interactive control that can display text, an optional
    icon, or a loading indicator.

    The button responds to pointer hover and press interactions with animated
    scaling. Its appearance can be customized through its public properties.

    Setting \l clickable to \c false disables interaction and displays a
    semi-transparent overlay. Setting \l loading to \c true replaces the
    button content with a loading indicator and suppresses click events.

    The \l pressed and \l hovered properties are inherited from \l T.Button
    and automatically respect \l clickable and \l loading through the
    \c enabled binding.

    \section1 Example Usage

    A button with text and an icon:

    \qml
    Revise.Button {
        text: qsTr("Save")
        icon.source: "save"
        onClicked: saveChanges()
    }
    \endqml

    A button displaying a loading indicator:

    \qml
    Revise.Button {
        text: qsTr("Save")
        loading: isSaving
        onClicked: saveChanges()
    }
    \endqml

    A customized button:

    \qml
    Revise.Button {
        text: qsTr("Continue")
        radius: 12
        color: "white"
        background.color: "#6750A4"
        font.bold: true
        horizontalAlignment: Qt.AlignHCenter
        clickable: !loading
    }
    \endqml
*/
T.Button {
    id: root

    /*!
        The corner radius of the button background.

        The default value is \c 6.
    */
    property int radius: 6

    /*!
        Whether the button responds to pointer interactions.

        When this property is \c false, the button does not emit
        the \l clicked signal and displays a semi-transparent overlay.

        The default value is \c true.
    */
    property bool clickable: true

    /*!
        Whether the button displays a loading indicator instead of
        its text and icon.

        Click events are suppressed while this property is \c true.

        The default value is \c false.
    */
    property bool loading: false

    /*!
        \qmlproperty color Button::color

        The color of the button text.
    */
    property alias color: txt.label.color

    /*!
        \qmlproperty color Button::backgroundItem

        The background of button.
    */
    property alias backgroundItem: bg

    /*!
        \qmlproperty enumeration Button::horizontalAlignment

        The horizontal alignment of the button content.

        Possible values are \c Qt.AlignLeft, \c Qt.AlignHCenter,
        and \c Qt.AlignRight.
    */
    property alias horizontalAlignment: txt.label.horizontalAlignment

    /*!
        \qmlproperty enumeration Button::verticalAlignment

        The vertical alignment of the button content.

        Possible values are \c Qt.AlignTop, \c Qt.AlignVCenter,
        and \c Qt.AlignBottom.
    */
    property alias verticalAlignment: txt.label.verticalAlignment

    font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    enabled: root.clickable && !root.loading

    padding: 8

    implicitWidth: txt.implicitWidth + 40
    implicitHeight: Math.max(35, txt.implicitHeight + 10)

    scale: {
        if (root.pressed) {
            return 0.96
        } else if (root.hovered) {
            return 0.98
        }
        return 1.0
    }

    layer.enabled: true
    layer.effect: Revise.Shadow {}

    background: Rectangle {
        id: bg
        anchors.fill: parent
        radius: root.radius
        color: Revise.Theme.surfaceContainerColor
    }

    contentItem: Revise.IconText {
        id: txt
        visible: root.clickable && !root.loading
        label.text: root.text
        font: root.font
        icon.source: root.icon.source
        label.horizontalAlignment: Qt.AlignHCenter
        label.verticalAlignment: Qt.AlignVCenter
    }

    Rectangle {
        visible: !root.clickable
        anchors.fill: parent
        radius: root.radius
        color: "black"
        opacity: 0.3
    }

    Revise.Spinner {
        visible: root.clickable && root.loading
        size: txt.height
        anchors.centerIn: parent
    }

    HoverHandler {
        cursorShape: root.hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }
}
