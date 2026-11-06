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
    read-only. This makes the component suitable for showing build
    information, identifiers, and other values that users may want to copy.

    If the key does not fit into the space that remains after the value takes
    its natural width, the key is elided on the right.

    \section1 Example Usage

    \qml
    Revise.KeyValue {
        key.text: qsTr("Compiler")
        value.text: Revise.BuildInfo.compilerId
    }
    \endqml
*/
Row {
    id: root

    width: parent ? parent.width : implicitWidth

    /*!
        \qmlproperty Revise.Text KeyValue::key
        \brief The label element shown on the left side of the row.

        The text is rendered with
        \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}
        by default. When the key is wider than the space that remains after
        the value, it is elided on the right so it never overlaps the value.
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
        clipboard. The element takes its natural width (\c implicitWidth),
        so its position on the right is stable regardless of how long the key
        is.
    */
    property alias value: valueText

    Revise.Text {
        id: keyText
        width: Math.max(0, root.width - valueText.width)
        color: Revise.Theme.foregroundColor
        elide: Text.ElideRight
    }

    Revise.CopyableText {
        id: valueText
        color: Revise.Theme.secondaryForegroundColor
    }
}
