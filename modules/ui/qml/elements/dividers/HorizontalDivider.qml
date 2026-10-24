// HorizontalDivider.qml

import QtQuick
import Revise as Revise

/**
 * \qmltype HorizontalDivider
 * \brief A thin horizontal rule used to separate vertical content elements or sections.
 *
 * The \c HorizontalDivider provides a clean visual boundary between stacked items.
 * By default, it features a height of \c 2 pixels with rounded edges and automatically
 * styles itself using a lighter shade of the current theme's surface color.
 *
 * \b {Example Usage:}
 * \code
 * Column {
 *     width: 250
 *     spacing: 8
 *
 *     Revise.Text { text: "Top Section" }
 *
 *     HorizontalDivider { width: parent.width }
 *
 *     Revise.Text { text: "Bottom Section" }
 * }
 * \endcode
 */
Rectangle {
    width: 100
    height: 2
    radius: 1
    color: Color.lighter(Revise.Theme.surfaceColor, 1.3)
}
