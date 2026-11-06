// KeyValue.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype KeyValue
    \brief A single key-value row with a label on the left and a value on the right.
    \inqmlmodule Revise
    \since 1.0

    KeyValue is a compact horizontal layout for showing a parameter name and
    its value. The key is rendered on the left with
    \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}, and the
    value is rendered on the right with
    \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor},
    visually de-emphasising the value relative to the label.

    Both the key and the value are exposed as aliases to the internal elements,
    so their text, font, and color can be adjusted from the outside without
    replacing the components. The value is a
    \l {Revise.CopyableText}{Revise.CopyableText}, which means it can be
    selected with the mouse and copied to the clipboard, while remaining
    read-only.

    The width of the key can be fixed through \l keyWidth, which is useful
    when several rows are stacked vertically and their keys should align in
    a column. When \l keyWidth is not set, the key takes the space that
    remains after the value, and is elided on the right if it does not fit.

    \section1 Example Usage

    \qml
    Revise.KeyValue {
        key.text: qsTr("Compiler")
        value.text: Revise.BuildInfo.compilerId
    }
    \endqml

    Aligning keys in a column:

    \qml
    Column {
        property int optionsWidth: 140

        Revise.KeyValue {
            width: parent.width
            keyWidth: parent.optionsWidth
            key.text: qsTr("Compiler")
            value.text: Revise.BuildInfo.compilerId
        }
    }
    \endqml
*/
Row {
    id: root

    width: parent ? parent.width : implicitWidth

    /*!
        \qmlproperty int KeyValue::keyWidth
        \brief The fixed width of the key, in pixels.

        When the value is greater than \c 0, the key is given exactly this
        width and the value takes the remaining space. This is useful when
        several rows are stacked and their keys should line up in a column.

        When the value is \c 0 (the default), the key takes the space that
        remains after the value has taken its natural width.

        The default value is \c 0.
    */
    property int keyWidth: 0

    /*!
        \qmlproperty Revise.Text KeyValue::key
        \brief The label element shown on the left side of the row.

        The text is rendered with
        \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}
        by default. The element is elided on the right if it does not fit
        into its width, so it never overlaps the value.
    */
    property alias key: keyText

    /*!
        \qmlproperty Revise.CopyableText KeyValue::value
        \brief The value element shown on the right side of the row.

        The text is rendered with
        \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor}
        by default. The element is a
        \l {Revise.CopyableText}{Revise.CopyableText}: it is read-only, but
        its contents can be selected with the mouse and copied to the
        clipboard.
    */
    property alias value: valueText

    Revise.Text {
        id: keyText
        width: root.keyWidth > 0
            ? root.keyWidth
            : Math.max(0, root.width - valueText.width)
        color: Revise.Theme.foregroundColor
        elide: Text.ElideRight
    }

    Revise.CopyableText {
        id: valueText
        width: root.keyWidth > 0
            ? Math.max(0, root.width - root.keyWidth)
            : implicitWidth
        color: Revise.Theme.secondaryForegroundColor
    }
}
