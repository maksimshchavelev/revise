// VerticalDivider.qml

import QtQuick
import Revise as Revise

/**
 * \qmltype VerticalDivider
 * \brief A thin vertical rule used to separate horizontal content elements or sections.
 *
 * The \c VerticalDivider provides a clean visual boundary between side-by-side items.
 * By default, it features a width of \c 2 pixels with rounded edges and automatically
 * styles itself using a lighter shade of the current theme's surface color.
 *
 * \b {Example Usage:}
 * \code
 * Row {
 *     height: 40
 *     spacing: 8
 *
 *     Revise.Button { text: "Back" }
 *
 *     VerticalDivider { height: parent.height }
 *
 *     Revise.Button { text: "Next" }
 * }
 * \endcode
 */
Rectangle {
    width: 2
    height: 100
    radius: 1
    color: Color.lighter(Revise.Theme.surfaceColor, 1.3)
}
