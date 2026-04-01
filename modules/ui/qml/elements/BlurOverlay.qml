// This element takes a source element, creates a blurred copy of it, and provides access to it. The blurred copy is invisible by default

import QtQuick
import QtQuick.Effects

Item {
    id: root

    property Item sourceItem: null
    property double blur: 1.2
    property alias blurredItem: blurredBackground

    ShaderEffectSource {
        id: bgSource
        sourceItem: root.sourceItem
        anchors.fill: parent
        live: true
        hideSource: false
        visible: false
    }

    MultiEffect {
        id: blurredBackground
        anchors.fill: parent
        source: bgSource
        blur: root.blur
        blurEnabled: true
        visible: false
    }
}
