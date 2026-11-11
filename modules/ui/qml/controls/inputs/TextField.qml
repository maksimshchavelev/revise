// TextField.qml

import QtQuick
import QtQuick.Templates as T
import Revise as Revise

/*!
    \qmltype TextField
    \brief A single-line text input field with a multi-colored border and soft validation.
    \inqmlmodule Revise
    \inherits T.TextField
    \since 1.0

    TextField provides a styled single-line input. It supports a placeholder text,
    an optional validator, and soft validation via a custom function. The border
    color changes based on the validation state. The component is fully editable
    on both desktop and mobile platforms.

    \section1 Example Usage

    \qml
    Revise.TextField {
        width: 200
        placeholder.text: qsTr("Enter something")
        validatorFunction: (text) => text.length > 3
    }
    \endqml
*/
T.TextField {
    id: root

    /*!
        \qmlproperty Revise.Text TextField::placeholder
        \brief The placeholder text element displayed when the field is empty.
    */
    property alias placeholder: placeholderItem

    /*!
        \qmlproperty var TextField::validatorFunction
        \brief A function used for soft validation without preventing input.

        The function receives the current text and returns a boolean.
    */
    property var validatorFunction: null

    /*!
        \qmlproperty bool TextField::valid
        \brief Whether the current text passes the soft validation.
        \readonly
    */
    readonly property bool valid: isValid()

    /*!
        \qmlproperty bool TextField::editable
        \brief Whether the field allows user input.

        When \c false, the field is read-only and displays a semi-transparent overlay.
    */
    property bool editable: true

    /*!
        \qmlproperty font TextField::font
        \brief The font used for the input text and placeholder.
    */
    property alias font: root.font

    font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    implicitHeight: 35
    leftPadding: 5
    rightPadding: 5
    topPadding: 5
    bottomPadding: 5

    verticalAlignment: TextInput.AlignVCenter
    horizontalAlignment: TextInput.AlignLeft

    selectByMouse: true
    color: Revise.Theme.foregroundColor
    selectionColor: Revise.Theme.primaryColor
    selectedTextColor: Revise.Theme.foregroundColor

    readOnly: !root.editable

    background: Rectangle {
        implicitHeight: 35
        radius: 6
        color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
        border.width: 1
        border.color: {
            if (!root.valid)
                return Revise.Theme.red
            return Color.lighter(Revise.Theme.surfaceColor, 1.5)
        }
        clip: true

        layer.effect: Revise.Shadow {}

        Revise.Text {
            id: placeholderItem
            anchors.fill: parent
            anchors.margins: 5
            verticalAlignment: Revise.Text.AlignVCenter
            horizontalAlignment: Revise.Text.AlignLeft
            font: root.font
            color: Color.darker(Revise.Theme.foregroundColor, 2)
            visible: !root.displayText.length && !root.preeditText && (!root.activeFocus || root.horizontalAlignment !== Text.AlignHCenter)
            elide: Revise.Text.ElideRight
        }
    }

    Rectangle {
        color: "black"
        opacity: 0.35
        anchors.fill: parent
        visible: !root.editable
        radius: 6
        z: 2
    }

    HoverHandler {
        cursorShape: hovered && root.editable ? Qt.IBeamCursor : Qt.ArrowCursor
    }

    onDisplayTextChanged: {
        if (root.validatorFunction) {
            root.valid = validatorFunction(root.displayText)
        }
    }

    function isValid() {
        if (!root.validatorFunction)
            return true

        return validatorFunction(root.displayText)
    }
}
