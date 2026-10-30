// Button.qml


import QtQuick
import Revise as Revise

/*!
    \qmltype Button
    \brief A button with optional text, icon, and loading indicator.
    \inqmlmodule Revise
    \inherits Item

    Button provides an interactive control that can display text, an optional
    icon, or a loading indicator.

    The button responds to pointer hover and press interactions with animated
    scaling. Its appearance can be customized through its public properties.

    Setting \l clickable to \c false disables interaction and displays a
    semi-transparent overlay. Setting \l loading to \c true replaces the
    button content with a loading indicator and suppresses click events.

    \section1 Example Usage

    A button with text and an icon:

    \qml
    Revise.Button {
        text: qsTr("Save")
        icon: "save"
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
Item {
    id: root

    /*!
        The corner radius of the button background.

        The default value is \c 6.
    */
    property int radius: 6

    /*!
        Whether the button is currently pressed.

        This property is \c true while the pointer is pressing the button
        and \l clickable is \c true and \l loading is \c false.
    */
    property bool pressed: tapHandler.pressed && clickable && !loading

    /*!
        Whether the pointer is hovering over the button.

        This property is \c true while the pointer is over the button
        and \l clickable is \c true and \l loading is \c false.
    */
    property bool hovered: hoverHandler.hovered && clickable && !loading

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
        \qmlproperty Rectangle Button::background

        The background rectangle of the button.

        Use this property to customize the background appearance,
        for example, by changing its color or corner radius.
    */
    property alias background: background

    /*!
        \qmlproperty string Button::text

        The text displayed by the button.
    */
    property alias text: txt.label.text

    /*!
        \qmlproperty color Button::color

        The color of the button text.
    */
    property alias color: txt.label.color

    /*!
        The font used to display the button text.
    */
    property alias font: txt.font

    /*!
        \qmlproperty var Button::icon

        The icon displayed by the button.

        The accepted value is determined by the \l Revise::IconText component.
    */
    property alias icon: txt.icon

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

    /*!
        This signal is emitted when the button is clicked while
        \l clickable is \c true and \l loading is \c false.

        The corresponding handler is \c onClicked.
    */
        signal
    clicked

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

    Rectangle {
        id: background
        anchors.fill: parent
        radius: root.radius
        color: Revise.Theme.surfaceContainerColor
    }

    Revise.IconText {
        id: txt
        visible: root.clickable && !root.loading
        anchors.fill: parent
        anchors.margins: 8
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
        id: hoverHandler
        cursorShape: root.hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    TapHandler {
        id: tapHandler
        onTapped: {
            if (root.clickable && !root.loading) {
                root.clicked()
            }
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }
}

