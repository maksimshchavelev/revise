// SettingsRow.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype SettingsRow
    \inqmlmodule Revise
    \brief A responsive settings row placing a label on the left and a control on the right.

    The SettingsRow component provides a standard UI layout for application settings.
    It features an auto-wrapping description text on the left and an interactive
    control widget on the right. It dynamically recalculates its layout upon window
    resizing to prevent content overlapping and layout clipping.

    Example of a basic toggle setting:
    \code
    Revise.SettingsRow {
        width: parent.width
        text: qsTr("Enable Notifications")

        Revise.Switch {
            checked: true
        }
    }
    \endcode

    Example of multiple rows grouped inside a panel:
    \code
    Revise.PanelColumn {
        width: 400

        Revise.SettingsRow {
            text: qsTr("Response time limit (seconds)")

            Revise.SpinBox {
                minimum: 1
                maximum: 60
                value: 15
            }
        }
    }
    \endcode
*/
Item {
    id: root

    /*!
        This property holds the description text displayed on the left side of the row.
     */
    property alias text: label.text

    /*!
        This property holds the interactive control element (e.g., Switch, SpinBox)
        placed on the right side. This is the default property.
     */
    default property alias control: controlSlot.data

    width: parent ? parent.width : 0
    height: Math.max(label.implicitHeight, controlSlot.height)

    Revise.Text {
        id: label

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: controlSlot.left
        anchors.rightMargin: 12

        wrapMode: Text.WordWrap
        width: Math.min(Revise.Theme.settingsLabelWidth, parent.width - controlSlot.width - anchors.rightMargin)
    }

    Item {
        id: controlSlot

        anchors.top: parent.top
        anchors.right: parent.right

        width: childrenRect.width
        height: childrenRect.height

        layer.enabled: true
    }
}
