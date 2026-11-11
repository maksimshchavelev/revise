// SettingsRow.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype SettingsRow
    \inqmlmodule Revise
    \inherits Loader
    \since 1.0

    \brief A responsive settings row that switches between desktop and mobile layouts.

    The SettingsRow component provides a standard UI layout for application settings.
    It automatically selects the appropriate layout based on the platform or user preferences
    through the \l {Revise.SettingsService::useMobileUi}{Revise.SettingsService.useMobileUi} property.

    On desktop, it places the description text on the left and the control on the right using
    \l DesktopSettingsRow. On mobile, it stacks the description text on top and the control below
    using \l MobileSettingsRow.

    Because the root element is a \l Loader, the control cannot be declared as a default child.
    It must be explicitly assigned using the \l control property.

    \section1 Example Usage

    \qml
    Revise.SettingsRow {
        width: parent.width
        text: qsTr("Enable Notifications")

        control: Revise.Switch {
            checked: true
        }
    }
    \endqml

    \section1 Example of multiple rows grouped inside a panel

    \qml
    Revise.PanelColumn {
        width: 400

        Revise.SettingsRow {
            text: qsTr("Response time limit (seconds)")

            control: Revise.SpinBox {
                minimum: 1
                maximum: 60
                value: 15
            }
        }
    }
    \endqml
*/
Loader {
    id: root

    /*!
        \qmlproperty string SettingsRow::text
        \brief The description text displayed on the row.

        This property is forwarded to the active layout implementation.
    */
    property string text

    /*!
        \qmlproperty Item SettingsRow::control
        \brief The interactive control element displayed in the row.

        This property is forwarded to the active layout implementation.
        The control is automatically reparented to the correct layout slot.
    */
    property Item control

    width: parent ? parent.width : 0

    sourceComponent: Revise.SettingsService.useMobileUi ? mobileComponent : desktopComponent

    Component {
        id: desktopComponent
        Revise.DesktopSettingsRow { }
    }

    Component {
        id: mobileComponent
        Revise.MobileSettingsRow { }
    }

    Binding {
        target: root.item
        property: "text"
        value: root.text
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "control"
        value: root.control
        when: root.item !== null
    }
}
