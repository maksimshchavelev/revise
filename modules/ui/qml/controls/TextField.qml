// Single-line text input field. You can set a validator. Multicolored border is present.

import QtQuick
import Revise as Revise

Rectangle {
    id: root

    property alias placeholder: placeholder
    property alias text: input.text
    property alias validator: input.validator
    property alias inputMethodHints: input.inputMethodHints

    property var validatorFunction: null // This function is used for soft validation without preventing input
    property bool valid: true
    property bool editable: true

    property font font: Qt.font({
        "family": Revise.Fonts.regular.name,
        "pixelSize": Revise.Theme.fontSizeDefault
    })

    implicitHeight: 35
    radius: 6
    color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
    border.width: 1
    border.color: Color.lighter(Revise.Theme.surfaceColor, 1.5)
    clip: true

    layer.enabled: true
    layer.effect: Revise.Shadow {}

    Revise.Text {
        id: placeholder

        text: qsTr("Введите что-нибудь")
        anchors.fill: parent
        anchors.margins: 5
        verticalAlignment: Revise.Text.AlignVCenter
        horizontalAlignment: Revise.Text.AlignLeft
        font: root.font
        color: Color.darker(Revise.Theme.foregroundColor, 2)
        visible: root.text.length === 0
        elide: Revise.Text.ElideRight
    }

    TextInput {
        id: input

        anchors.fill: parent
        anchors.margins: 5
        verticalAlignment: Revise.Text.AlignVCenter
        horizontalAlignment: Revise.Text.AlignLeft
        font: root.font
        onTextChanged: root.valid = root.isValid()
        color: Revise.Theme.textColor
        readOnly: !root.editable
        clip: true
        enabled: false
    }

    Rectangle {
        color: "black"
        opacity: 0.35
        anchors.fill: parent
        visible: !root.editable
        radius: root.radius
    }

    HoverHandler {
        cursorShape: hovered && root.editable ? Qt.IBeamCursor : Qt.ArrowCursor
    }

    TapHandler {
        onTapped: function (point) {
            input.enabled = true
            input.forceActiveFocus()
            input.cursorPosition = input.positionAt(point.position.x,
                point.position.y)
            Qt.inputMethod.show()
        }
    }

    Connections {
        target: input

        function onActiveFocusChanged() {
            if (!input.activeFocus)
                input.enabled = false
        }
    }

    function isValid() {
        if (!root.validatorFunction) {
            return true
        }

        return validatorFunction(root.text)
    }
}
