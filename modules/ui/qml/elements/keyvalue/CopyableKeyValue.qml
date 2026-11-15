// CopyableKeyValue.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CopyableKeyValue
    \brief A copyable key-value pair that can be laid out horizontally or vertically.
    \inqmlmodule Revise
    \inherits Grid
    \since 1.0

    CopyableKeyValue pairs a key with a value, rendered using
    \l {Revise.CopyableText}{Revise.CopyableText}. By default, it arranges
    them in a horizontal row, allowing the value to take up the remaining
    space.

    If \l isVertical is set to \c true, the elements are stacked in a
    column, and both the key and the value expand to fill the available width.

    \section1 Example Usage

    A basic horizontal row:

    \qml
    Revise.CopyableKeyValue {
        key.text: qsTr("Deck ID")
        value.text: deckId
    }
    \endqml

    A vertical layout:

    \qml
    Revise.CopyableKeyValue {
        width: parent.width
        isVertical: true
        key.text: qsTr("Deck ID")
        value.text: deckId
    }
    \endqml
*/
Grid {
    id: root

    width: parent ? parent.width : implicitWidth

    /*!
        \qmlproperty bool CopyableKeyValue::isVertical
        \brief Whether the key and value are stacked vertically.

        When \c true, the layout acts as a \l Column, and both elements
        expand to fill the width. When \c false, it acts as a \l Row.

        The default value is \c false.
    */
    property bool isVertical: false

    /*!
        \qmlproperty int CopyableKeyValue::keyWidth
        \brief The fixed width of the key, in pixels.

        This property only applies when \l isVertical is \c false. When the
        value is greater than \c 0, the key is given exactly this width and
        the value takes the remaining space. When the value is \c 0 (the default),
        the key takes its natural width.

        The default value is \c 0.
    */
    property int keyWidth: 0

    /*!
        \qmlproperty Revise.CopyableText CopyableKeyValue::key
        \brief The copyable label element.
    */
    property alias key: keyText

    /*!
        \qmlproperty Revise.CopyableText CopyableKeyValue::value
        \brief The copyable value element.
    */
    property alias value: valueText

    columns: root.isVertical ? 1 : -1
    rows: root.isVertical ? -1 : 1
    spacing: root.isVertical ? 4 : 0

    Revise.CopyableText {
        id: keyText
        width: root.isVertical
            ? root.width
            : (root.keyWidth > 0 ? root.keyWidth : Math.max(0, root.width - valueText.width))
        color: Revise.Theme.foregroundColor
    }

    Revise.CopyableText {
        id: valueText
        width: root.isVertical
            ? root.width
            : (root.keyWidth > 0 ? Math.max(0, root.width - root.keyWidth) : implicitWidth)
        color: Revise.Theme.secondaryForegroundColor
    }
}
