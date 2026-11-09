// SpinBox.qml

import QtQuick
import QtQuick.Controls.Basic
import Revise as Revise

/*!
    \qmltype SpinBox
    \brief A numeric input component with a text field that restricts entries to integers.
    \inqmlmodule Revise
    \inherits SpinBox
    \since 1.0

    This control replicates the visual style of Revise text inputs while providing rigorous integer
    validation. It uses the standard \l {SpinBox::from}{from}, \l {SpinBox::to}{to}, and
    \l {SpinBox::value}{value} properties inherited from the base \l SpinBox type to manage the
    numeric range.

    The text field uses \l {SpinBox::displayText}{displayText} for synchronization, which prevents
    recursive binding loops and ensures correct behaviour across platforms, including Android
    and Windows. Interaction is handled natively by the control without manual focus or input
    method management.

    \section1 Example Usage

    \qml
    Revise.SpinBox {
        from: 1
        to: 3600
        value: 60
    }
    \endqml
*/
SpinBox {
    id: root

    font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    width: 120
    implicitHeight: 35
    clip: true

    editable: true

    up.indicator: Item {}
    down.indicator: Item {}

    contentItem: TextInput {
        id: input

        anchors.fill: parent
        anchors.margins: 5
        text: root.displayText
        font: root.font
        color: Revise.Theme.foregroundColor
        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: root.inputMethodHints
        verticalAlignment: Revise.Text.AlignVCenter
        horizontalAlignment: Revise.Text.AlignLeft
        clip: true
    }

    background: Rectangle {
        radius: 6
        color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
        border.width: 1
        border.color: Color.lighter(Revise.Theme.surfaceColor, 1.5)

        layer.effect: Revise.Shadow {}

        Rectangle {
            color: "black"
            opacity: 0.35
            anchors.fill: parent
            visible: !root.editable
            radius: parent.radius
        }
    }

    HoverHandler {
        cursorShape: hovered && root.editable ? Qt.IBeamCursor : Qt.ArrowCursor
    }
}
