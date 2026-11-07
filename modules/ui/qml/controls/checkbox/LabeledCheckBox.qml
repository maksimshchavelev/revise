// LabeledCheckBox.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype LabeledCheckBox
    \brief A check box with a text label placed to its right.
    \inqmlmodule Revise
    \inherits Row
    \since 1.0

    LabeledCheckBox combines a \l Revise.CheckBox with a \l Revise.Text label
    laid out horizontally, so the label sits to the right of the box and is
    vertically centered with it.

    \section1 Example Usage

    \qml
    Revise.LabeledCheckBox {
        width: 240
        text: qsTr("Enable notifications")
        onChanged: (checked) => settings.notificationsEnabled = checked
    }
    \endqml
*/
Row {
    id: root

    /*!
        \qmlproperty bool LabeledCheckBox::checked
        \brief Whether the check box is checked.

        This property is an alias to the internal
        \l {Revise.CheckBox::checked}{Revise.CheckBox.checked}, so reading and
        writing it has the same effect as interacting with the check box
        directly.

        The default value is \c false.
    */
    property alias checked: checkbox.checked

    /*!
        \qmlproperty string LabeledCheckBox::text
        \brief The text shown next to the check box.

        The label is wrapped, limited to two lines, and elided on the right
        if it does not fit into the space that remains to the right of the
        check box.
    */
    property string text

    /*!
        \qmlproperty color LabeledCheckBox::color
        \brief The color associated with the labeled check box.

        The property is declared for consumers but is not applied to the
        label in the current implementation.

        The default value is
        \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}.
    */
    property color color: Revise.Theme.foregroundColor

    /*!
        \qmlsignal LabeledCheckBox::changed(bool checked)
        \brief Emitted when the user toggles the check box.

        The \a checked argument contains the new state of the check box.
        The signal is forwarded from the internal
        \l {Revise.CheckBox::changed}{Revise.CheckBox.changed}.

        The corresponding handler is \c onChanged.
    */
    signal changed(bool checked);

    width: 100
    spacing: 8

    Revise.CheckBox {
        id: checkbox
        anchors.verticalCenter: label.verticalCenter

        onChanged: function(checked) {
            root.changed(checked)
        }
    }

    Revise.Text {
        id: label
        width: root.width - root.spacing - checkbox.width
        wrapMode: Revise.Text.Wrap
        elide: Revise.Text.ElideRight
        maximumLineCount: 2
        text: root.text
    }
}
