// MobileSettingsRow.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype MobileSettingsRow
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    \brief A settings row with a description on top and a control below.
    \brief Mobile implementation of the responsive settings row.

    MobileSettingsRow provides a vertical layout for application settings. It places
    a wrapping description text on the top and an interactive control widget below it.
    The control is placed inside a dedicated slot that aligns it to the right side of the row.

    This component is typically used internally by \l SettingsRow and is not meant
    to be instantiated directly.

    \section1 Example Usage

    \qml
    Revise.MobileSettingsRow {
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
        \qmlproperty string MobileSettingsRow::text
        \brief The description text displayed on the top side of the row.

        The text wraps automatically to fit the width of the row.
    */
    property alias text: label.text

    /*!
        \qmlproperty Item MobileSettingsRow::control
        \brief The interactive control element placed below the text.

        The control is automatically reparented to the internal slot when this property is set.
    */
    property Item control

    width: parent ? parent.width : 0
    height: layout.implicitHeight

    Column {
        id: layout
        width: parent.width
        spacing: 12

        Revise.Text {
            id: label
            width: parent.width
            wrapMode: Text.Wrap
        }

        Item {
            id: controlSlot
            anchors.right: parent.right
            width: childrenRect.width
            height: childrenRect.height
        }
    }

    onControlChanged: {
        if (root.control && root.control.parent !== controlSlot) {
            root.control.parent = controlSlot
        }
    }
}
