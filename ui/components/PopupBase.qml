// Base of popup

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls as QQC
import Revise

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property Item sourceItem // for blurring
    property real maxHeightRatio: 0.8 // When content exceeds this ratio, enable scrolling

    default property alias content: contentItem.data

    ShaderEffectSource {
        id: snapshot
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: true
        recursive: true
        hideSource: false
        smooth: true
    }

    MultiEffect {
        anchors.fill: parent
        source: snapshot
        blurEnabled: true
        blur: 1
        blurMax: 32
        autoPaddingEnabled: false
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.25
    }

    // Main popup container
    Item {
        id: popupContainer
        width: Math.min(parent.width * 0.85, 420)
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        // Calculate if scrolling is needed
        property bool scrollingNeeded: contentItem.implicitHeight > (root.height * maxHeightRatio)
        property real targetHeight: scrollingNeeded ?
            (root.height * maxHeightRatio) :
            Math.min(contentItem.implicitHeight + contentItem.anchors.margins * 2, root.height * maxHeightRatio)

        // Background panel
        Rectangle {
            id: panel
            anchors.fill: parent
            color: "black"
            opacity: 0.6
            radius: 20
            border.color: Theme.popupBorder
            border.width: 2
        }

        Flickable {
            contentWidth: popupContainer.width
            contentHeight: contentItem.implicitHeight + contentItem.anchors.margins * 2
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick
            clip: true
            anchors.fill: parent
            anchors.margins: panel.border.width + 1

            ColumnLayout {
                id: contentItem
                anchors.fill: parent
                spacing: 12
                anchors.margins: 16
            }
        }

        // Dynamic height binding - updates when content changes
        height: popupContainer.targetHeight

        // Animation for smooth height transitions
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutCubic
                duration: 180
            }
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 100
        }
    }

    function _openBase() {
        root.visible = true
        popupContainer.height = Qt.binding(function() { return popupContainer.targetHeight })
    }

    function _closeBase() {
        root.visible = false
    }

    DebugBounds {}
}

