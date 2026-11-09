// Tooltip.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype Tooltip
    \brief A popup that displays a short, contextual hint.
    \inqmlmodule Revise
    \inherits Popup
    \since 1.0

    Tooltip is a non-modal popup used to display descriptive text or hints near an interactive element. It automatically wraps the text, limits the number of lines to prevent overly large tooltips, and scales its padding and font size according to the global UI scale setting.

    \section1 Example Usage

    \qml
    Revise.Tooltip {
        text: qsTr("This is a hint")
    }
    \endqml
*/
Popup {
    id: root

    /*!
        \qmlproperty string Tooltip::text
        \brief The text displayed inside the tooltip.
    */
    property string text

    /*!
        \qmlproperty int Tooltip::maxWidth
        \brief The maximum width of the tooltip content, in pixels.

        The actual width is scaled by the global UI scale setting.

        The default value is \c 300.
    */
    property int maxWidth: 300

    readonly property real _uiScale: Revise.SettingsService.uiScale
    readonly property real _maxContentWidth: maxWidth * _uiScale

    closePolicy: Popup.NoAutoClose
    modal: false
    focus: true
    padding: 10 * _uiScale

    background: Rectangle {
        radius: root.height * 0.35
        color: Color.darker(Revise.Theme.backgroundColor, 3)
    }

    contentItem: Revise.Text {
        id: textItem

        text: root.text
        wrapMode: textMetrics.advanceWidth > root._maxContentWidth
            ? Text.Wrap
            : Text.NoWrap
        width: Math.min(textMetrics.advanceWidth, root._maxContentWidth)
        elide: Text.ElideRight
        maximumLineCount: 10
        font.pixelSize: Revise.Theme.fontSizeSmall * root._uiScale

        TextMetrics {
            id: textMetrics
            font: textItem.font
            text: textItem.text
        }
    }
}

