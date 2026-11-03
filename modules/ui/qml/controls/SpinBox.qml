// SpinBox.qml

import QtQuick
import QtQuick.Controls as Controls
import Revise as Revise

/**
 * \qmltype SpinBox
 * \brief A numeric input component with a text field that restricts entries to integers within a configurable range.
 *
 * This control replicates the visual style of Revise text inputs while providing rigorous integer
 * validation. It automatically bounds the numeric value between minimum and maximum constraints
 * and safely handles empty or non-numeric inputs.
 *
 * \b {Example Usage:}
 * \code
 * SpinBox {
 *     minimum: 1
 *     maximum: 3600
 *     value: 60
 *     placeholder: "From 1 to 1000"
 * }
 * \endcode
 */
Controls.SpinBox {
    id: root

    /**
     * \qmlproperty int SpinBox::minimum
     * The minimum acceptable integer value.
     * The default value is \c 0.
     */
    property int minimum: 0

    /**
     * \qmlproperty int SpinBox::maximum
     * The maximum acceptable integer value.
     * The default value is \c 100.
     */
    property int maximum: 100

    /**
     * \qmlproperty string SpinBox::placeholder
     * The text that is displayed when the input field is empty.
     */
    property alias placeholder: placeholder.text

    font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    width: 120
    implicitHeight: 35
    clip: true

    from: root.minimum
    to: root.maximum
    editable: true

    up.indicator: Item {}
    down.indicator: Item {}

    contentItem: TextInput {
        id: input

        anchors.fill: parent
        anchors.margins: 5
        text: root.textFromValue(root.value, root.locale)
        font: root.font
        color: Revise.Theme.textColor
        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: root.inputMethodHints
        verticalAlignment: Revise.Text.AlignVCenter
        horizontalAlignment: Revise.Text.AlignLeft
        clip: true
        enabled: false

        Revise.Text {
            id: placeholder

            anchors.fill: parent
            verticalAlignment: Revise.Text.AlignVCenter
            horizontalAlignment: Revise.Text.AlignLeft
            font: root.font
            color: Color.darker(Revise.Theme.foregroundColor, 2)
            visible: input.text.length === 0
            elide: Revise.Text.ElideRight
        }
    }

    background: Rectangle {
        radius: 6
        color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
        border.width: 1
        border.color: Color.lighter(Revise.Theme.surfaceColor, 1.5)

        layer.enabled: true
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

    TapHandler {
        onTapped: function (point) {
            if (!root.editable) return
            input.enabled = true
            input.forceActiveFocus()
            input.cursorPosition = input.positionAt(point.position.x, point.position.y)
            InputMethod.show()
        }
    }

    Connections {
        target: input
        function onActiveFocusChanged() {
            if (!input.activeFocus)
                input.enabled = false
        }
    }
}

