// Icon.qml

import QtQuick
import QtQuick.VectorImage
import QtQuick.Effects

/*!
    \qmltype Icon
    \brief A tinted vector icon.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    Icon renders an SVG image at a fixed square size and tints it with a
    single color. The icon keeps its API minimal: \l source points to the
    image, \l color sets the tint, and \l size controls both the width and
    the height of the element.

    The SVG is rendered through \l {QtQuick.VectorImage::VectorImage}{VectorImage}
    with the curve renderer, so the image stays sharp when the UI is scaled
    or when the icon is resized. The tint is applied with
    \l {QtQuick.Effects::MultiEffect}{MultiEffect}, which colorizes the
    rendered image while preserving its alpha channel.

    \section1 Example Usage

    \qml
    Revise.Icon {
        size: 24
        source: "qrc:/res/img/settings.svg"
        color: Revise.Theme.primaryColor
    }
    \endqml

    \sa Revise.IconButton, Revise.IconText
*/
Item {
    id: root

    /*!
        \qmlproperty int Icon::size
        \brief The side length of the icon in pixels.

        The value is used for both \l width and \l height, so the element
        is always square. The default value is \c 16.
    */
    property int size: 16

    /*!
        \qmlproperty url Icon::source
        \brief The location of the SVG image to render.

        This property is an alias to the internal
        \l {QtQuick.VectorImage::VectorImage::source}{VectorImage.source}
        and accepts the same URL schemes, including \c qrc paths.
    */
    property alias source: img.source

    /*!
        \qmlproperty color Icon::color
        \brief The tint applied to the icon.

        The icon is rendered as a mask: every non-transparent pixel of the
        source image is painted with this color, while transparent pixels
        stay transparent. This property is an alias to
        \l {QtQuick.Effects::MultiEffect::colorizationColor}{MultiEffect.colorizationColor}.
    */
    property alias color: overlay.colorizationColor

    width: root.size
    height: root.size

    VectorImage {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        preferredRendererType: VectorImage.CurveRenderer
        antialiasing: true
        visible: false
    }

    MultiEffect {
        id: overlay
        anchors.fill: img
        source: img
        colorization: 1.0
    }
}
