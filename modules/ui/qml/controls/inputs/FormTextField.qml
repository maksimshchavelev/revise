import QtQuick
import Revise as Revise

/**
 * \qmltype FormTextField
 * \brief A compound control that combines a text label and a text input field stacked vertically.
 *
 * This component simplifies form creation by wrapping a \c Revise.Text label
 * and a \c Revise.TextField input inside a single \c Column layout. It automatically
 * handles widths and provides safe text eliding for the label.
 *
 * \b {Example Usage:}
 * \code
 * FormTextField {
 *     width: 300
 *     label.text: qsTr("Username")
 *     textField.placeholder.text: qsTr("Enter your name")
 *     textField.text: user.name
 * }
 * \endcode
 */
Column {
    id: root

    // Text above the text input field
    property alias label: label

    // The text input field itself
    property alias textField: textField

    width: 150
    spacing: 6

    Revise.Text {
        id: label
        maximumLineCount: 1
        width: parent.width
        elide: Revise.Text.ElideRight
    }

    Revise.TextField {
        id: textField
        width: parent.width
    }
}
