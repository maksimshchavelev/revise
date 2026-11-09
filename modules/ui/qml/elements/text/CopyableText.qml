// CopyableText.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CopyableText
    \brief A read-only text element that can be selected and copied.
    \inqmlmodule Revise
    \inherits TextEdit
    \since 1.0

    CopyableText renders text using the standard Revise font and color.
    Editing is disabled, but the text can be selected with the mouse and
    copied to the clipboard. The text cursor is hidden to visually indicate
    that the element is read-only.

    \section1 Example Usage

    \qml
    Revise.CopyableText {
        text: Revise.BuildInfo.compilerId
    }
    \endqml

    \sa Revise.CopyableKeyValue, Revise.Text
*/
TextEdit {
    id: root

    readOnly: true
    selectByMouse: true

    selectionColor: Revise.Theme.primaryColor
    selectedTextColor: Revise.Theme.foregroundColor
    wrapMode: TextEdit.Wrap

    color: Revise.Theme.foregroundColor
    font.pixelSize: Revise.Theme.fontSizeDefault
    font.family: Revise.Fonts.regular.name

    cursorDelegate: Item {
        width: 0
        height: root.height
    }
}
