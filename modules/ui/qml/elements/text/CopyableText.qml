// CopyableText.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CopyableText
    \brief A read-only text element whose contents can be selected and copied.
    \inqmlmodule Revise
    \inherits TextEdit
    \since 1.0

    CopyableText renders text with the same visual style as \l Revise.Text
    but allows the user to select fragments of the text with the mouse and
    copy them to the clipboard. Editing is disabled: the content can only
    be read and selected.

    The selection is highlighted with
    \l {Revise.Theme::primaryColor}{Revise.Theme.primaryColor}, while the
    selected text itself is drawn with
    \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor} so it
    stays legible on the selection background. The text cursor is hidden,
    so the element does not look editable even when it has keyboard focus.

    \section1 Example Usage

    \qml
    Revise.CopyableText {
        text: Revise.BuildInfo.compilerId
    }
    \endqml

    \sa Revise.Text, TextEdit
*/
TextEdit {
    id: root

    readOnly: true
    selectByMouse: true
    cursorVisible: false

    selectionColor: Revise.Theme.primaryColor
    selectedTextColor: Revise.Theme.foregroundColor

    color: Revise.Theme.foregroundColor
    font.pixelSize: Revise.Theme.fontSizeDefault
    font.family: Revise.Fonts.regular.name
}
