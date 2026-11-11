// DesktopSettingsRow.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype DesktopSettingsRow
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    \brief A settings row with a description on the left and a control on the right.
    \brief Desktop implementation of the responsive settings row.

    DesktopSettingsRow provides a horizontal layout for application settings. It places
    a wrapping description text on the left side and an interactive control widget on the right side.
    The control is placed inside a dedicated slot that automatically adjusts its width and height
    based on the control's implicit size.

    This component is typically used internally by \l SettingsRow and is not meant
    to be instantiated directly.

    \section1 Example Usage

    \qml
    Revise.DesktopSettingsRow {
        width: parent.width
        text: qsTr("Enable notifications")

        control: Revise.Switch {
            checked: true
        }
    }
    \endqml
*/
Item {
    id: root

    /*!
        \qmlproperty string DesktopSettingsRow::text
        \brief The description text displayed on the left side of the row.

        The text wraps automatically if it exceeds the available width.
    */
    property alias text: label.text

    /*!
        \qmlproperty Item DesktopSettingsRow::control
        \brief The interactive control element placed on the right side.

        The control is automatically reparented to the internal slot when this property is set.
    */
    property Item control

    width: parent ? parent.width : 0
    height: Math.max(label.implicitHeight, controlSlot.height)

    Revise.Text {
        id: label
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: controlSlot.left
        anchors.rightMargin: 12
        verticalAlignment: Revise.Text.AlignVCenter
        wrapMode: Text.Wrap
        width: Math.min(Revise.Theme.settingsLabelWidth, parent.width - controlSlot.width - anchors.rightMargin)
    }

    Item {
        id: controlSlot
        anchors.top: parent.top
        anchors.right: parent.right
        width: childrenRect.width
        height: childrenRect.height
    }

    onControlChanged: {
        if (root.control && root.control.parent !== controlSlot) {
            root.control.parent = controlSlot
        }
    }
}
