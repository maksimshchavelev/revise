// CopyableKeyValue.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CopyableKeyValue
    \brief A horizontal row displaying a copyable key-value pair.
    \inqmlmodule Revise
    \inherits Row
    \since 1.0

    CopyableKeyValue is a layout component that pairs a key on the left
    with a value on the right. Both the key and the value are rendered
    using \l {Revise.CopyableText}{Revise.CopyableText}, allowing users
    to select and copy the text of either element to the clipboard.

    The layout behavior can be controlled via \l keyWidth. By default,
    the value takes its natural width and the key fills the remaining
    space. When \l keyWidth is set to a positive value, the key is
    fixed to that width and the value takes the rest of the row.

    \section1 Example Usage

    A basic row with a copyable key and value:

    \qml
    Revise.CopyableKeyValue {
        key.text: qsTr("Deck ID")
        value.text: deckId
    }
    \endqml

    Aligning keys in a column:

    \qml
    Column {
        property int optionsWidth: 140

        Revise.CopyableKeyValue {
            width: parent.width
            keyWidth: parent.optionsWidth
            key.text: qsTr("Deck ID")
            value.text: deckId
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
        \qmlproperty Revise.CopyableText KeyValue::key
        \brief The copyable label element shown on the left side of the row.

        The text is rendered with
        \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}
        by default. The element is elided on the right if it does not fit
        into its width, so it never overlaps the value.
    */
    property alias key: keyText

    /*!
        \qmlproperty Revise.CopyableText KeyValue::value
        \brief The copyable value element shown on the right side of the row.

        The text is rendered with
        \l {Revise.Theme::secondaryForegroundColor}{Revise.Theme.secondaryForegroundColor}
        by default. The element is a
        \l {Revise.Text}{Revise.Text}: it is read-only
        clipboard.
    */
    property alias value: valueText

    Revise.CopyableText {
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
