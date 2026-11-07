// ColorfulProgressbar.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype ColorfulProgressbar
    \brief A horizontal progress bar whose fill color changes with progress.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    ColorfulProgressbar renders a rounded track with a rounded fill whose
    width is proportional to \l progress. The fill color is not fixed: it
    is interpolated between
    \l {Revise.Theme::progressbarLowColor}{Revise.Theme.progressbarLowColor},
    \l {Revise.Theme::progressbarMediumColor}{Revise.Theme.progressbarMediumColor},
    and
    \l {Revise.Theme::progressbarHighColor}{Revise.Theme.progressbarHighColor}
    as \l progress moves from \c 0 to \c 1, so the bar communicates both the
    amount and the level of progress at a glance.

    Both the fill width and the fill color are animated with short
    transitions, so rapid changes in \l progress do not produce abrupt
    jumps.

    \section1 Example Usage

    \qml
    Revise.ColorfulProgressbar {
        width: 240
        progress: 0.7
    }
    \endqml

    \sa Revise.RadialGauge
*/
Item {
    id: root

    /*!
        \qmlproperty real ColorfulProgressbar::progress
        \brief The current progress, in the range \c {[0, 1]}.
        \default 0

        A value of \c 0 renders an empty fill, \c 1 renders a full fill, and
        values in between render a proportional fill. Values outside the
        range are clamped for both the fill width and the fill color.
    */
    property real progress: 0 // 0 - 1

    width: 200
    height: 20

    Rectangle {
        radius: root.height / 2
        anchors.fill: parent
        color: Color.lighter(Revise.Theme.surfaceColor, 1.2)
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            radius: root.height / 2
            width: Math.min(root.width * root.progress, root.width)
            height: Math.min(root.height, width + radius)
            color: root.progressColor(root.progress)

            Behavior on width {
                NumberAnimation {
                    duration: 15
                }
            }

            Behavior on color {
                ColorAnimation {
                    duration: 15
                }
            }
        }
    }

    /*!
        \qmlmethod real ColorfulProgressbar::lerp(real a, real b, real t)
        \brief Linearly interpolates between \a a and \a b by factor \a t.

        Returns \c {a + (b - a) * t}. Used internally by
        \l lerpColor to interpolate color components.
    */
    function lerp(a, b, t) {
        return a + (b - a) * t
    }

    /*!
        \qmlmethod color ColorfulProgressbar::lerpColor(color c1, color c2, real t)
        \brief Linearly interpolates between colors \a c1 and \a c2 by factor \a t.

        Each component, including alpha, is interpolated with \l lerp.
        Used by \l progressColor to build the fill color.
    */
    function lerpColor(c1, c2, t) {
        return Qt.rgba(
            lerp(c1.r, c2.r, t),
            lerp(c1.g, c2.g, t),
            lerp(c1.b, c2.b, t),
            lerp(c1.a, c2.a, t)
        )
    }

    /*!
        \qmlmethod color ColorfulProgressbar::progressColor(real p)
        \brief Returns the fill color for progress value \a p.

        The value \a p is clamped to the range \c {[0, 1]}. For values below
        \c 0.5, the color is interpolated between
        \l {Revise.Theme::progressbarLowColor}{Revise.Theme.progressbarLowColor}
        and
        \l {Revise.Theme::progressbarMediumColor}{Revise.Theme.progressbarMediumColor}.
        For values at or above \c 0.5, it is interpolated between
        \l {Revise.Theme::progressbarMediumColor}{Revise.Theme.progressbarMediumColor}
        and
        \l {Revise.Theme::progressbarHighColor}{Revise.Theme.progressbarHighColor}.
    */
    function progressColor(p) {
        p = Math.max(0, Math.min(1, p))
        let t

        if (p < 0.5) {
            t = p / 0.5
            return lerpColor(
                Revise.Theme.progressbarLowColor,
                Revise.Theme.progressbarMediumColor,
                t
            )
        } else {
            t = (p - 0.5) / 0.5
            return lerpColor(
                Revise.Theme.progressbarMediumColor,
                Revise.Theme.progressbarHighColor,
                t
            )
        }
    }
}
