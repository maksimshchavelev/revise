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

    // for blurring (backgroundItem will be blurred)
    property Item backgroundItem

    property real maxHeightRatio: 0.8 // When content exceeds this ratio, enable scrolling
    property real padding: 16 // Content indents from popup borders
    property bool modal: true // If true, the popup will close when you click outside the popup area with emitting `closed` signal
    property real widthLimit: Math.min(parent.width * 0.85, 250)

    default property alias content: contentItem.data

    signal opened
    signal closed

    MultiEffect {
        anchors.fill: parent
        source: root.backgroundItem
        blurEnabled: true
        blur: 1
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.25
    }

    MouseArea {
        anchors.fill: parent
        onClicked: _closeBase()
    }

    // Main popup container
    Item {
        id: popupContainer
        width: root.widthLimit
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        // Calculate if scrolling is needed
        property bool scrollingNeeded: contentWrapper.height > (root.height * maxHeightRatio)
        property real targetHeight: scrollingNeeded ? (root.height * maxHeightRatio) : Math.min(
                                                          contentWrapper.height + root.padding * 2
                                                          + flickable.anchors.margins * 2,
                                                          root.height * maxHeightRatio)

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
            id: flickable
            contentWidth: popupContainer.width - anchors.margins * 2
            contentHeight: popupContainer.height - anchors.margins * 2
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick
            clip: true
            anchors.fill: parent
            anchors.margins: panel.border.width + 1

            // Wrapper for padding
            Item {
                id: contentWrapper

                width: flickable.width - root.padding * 2
                height: contentItem.implicitHeight

                anchors.centerIn: parent

                //anchors.margins: root.padding
                // width: parent.width
                // height: contentItem.implicitHeight + root.padding * 2
                ColumnLayout {
                    id: contentItem
                    spacing: 12

                    anchors.fill: parent

                    // anchors.margins: root.padding
                    onChildrenChanged: {
                        for (var i = 0; i < children.length; ++i) {
                            children[i].Layout.fillWidth = true
                            children[i].Layout.alignment = Qt.AlignHCenter | Qt.AlignVCenter

                            if (i == 0) {
                                children[i].Layout.topMargin = 0
                            } else if (i == children.length - 1) {
                                children[i].Layout.bottomMargin = 0
                            }
                        }
                    }
                }

                DebugBounds {
                    border.color: "green"
                }
            }

            DebugBounds {
                border.color: "yellow"
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

        DebugBounds {}
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 100
        }
    }

    function _openBase() {
        root.visible = true

        if (backgroundItem) {
            backgroundItem.visible = false
        }

        popupContainer.height = Qt.binding(function () {
            return popupContainer.targetHeight
        })
        root.opened()
    }

    function _closeBase() {
        root.visible = false

        if (backgroundItem) {
            backgroundItem.visible = true
        }

        root.closed()
    }

    DebugBounds {}
}
