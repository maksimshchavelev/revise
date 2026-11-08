// CopyableText.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CopyableText
    \brief A read-only single-line text element whose contents can be selected and copied.
    \inqmlmodule Revise
    \inherits TextInput
    \since 1.0

    CopyableText renders a single line of plain text with the same visual
    style as \l Revise.Text, while allowing the user to select fragments
    of the text with the mouse and copy them to the clipboard.

    Editing is disabled through \c readOnly. The text can be selected
    with the mouse using \c selectByMouse and copied using the standard
    text input keyboard shortcuts or the inherited \c copy() method.

    The selection is highlighted with
    \l {Revise.Theme::primaryColor}{Revise.Theme.primaryColor}, while
    the selected text itself is drawn with
    \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}
    so it stays legible on the selection background.

    The insertion cursor is visually disabled by providing a zero-width
    cursor delegate. This keeps the cursor hidden even when the element
    receives active focus.

    Unlike TextEdit, TextInput is designed for single-line text and
    does not provide a multi-line editing document or rich text editing
    functionality.

    \section1 Example Usage

    \qml
    Revise.CopyableText {
        text: Revise.BuildInfo.compilerId
    }
    \endqml

    \section1 Copying Selected Text

    The inherited \c copy() method copies the currently selected text
    to the system clipboard.

    \qml
    Revise.CopyableText {
        id: compilerText
        text: Revise.BuildInfo.compilerId

        Component.onCompleted: {
            selectAll()
            copy()
        }
    }
    \endqml

    \sa Revise.Text, TextInput, TextEdit
*/
TextInput {
    id: root

    readOnly: true
    selectByMouse: true

    selectionColor: Revise.Theme.primaryColor
    selectedTextColor: Revise.Theme.foregroundColor

    color: Revise.Theme.foregroundColor
    font.pixelSize: Revise.Theme.fontSizeDefault
    font.family: Revise.Fonts.regular.name

    cursorDelegate: Item {
        width: 0
        height: root.height
    }
}
