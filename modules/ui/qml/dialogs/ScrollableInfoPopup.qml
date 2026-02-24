// A pop-up notification with adaptive height and scrollable content when needed

import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Effects
import Revise

Item {
    id: root

    anchors.fill: parent
    visible: false
    z: 1000
    opacity: root.visible ? 1 : 0

    property string text
    property Item sourceItem
    property real maxHeightRatio: 0.85 // When content exceeds this ratio, enable scrolling

    // Modal background with click blocker
    property bool modal: true

    // Background blur effect
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

    // Dimming layer with click blocker
    Rectangle {
        id: dimmer
        anchors.fill: parent
        color: "black"
        opacity: 0.25

        MouseArea {
            anchors.fill: parent
            enabled: root.modal && root.visible
            hoverEnabled: true
            onClicked: {}
            onPressed: {}
            onReleased: {}
        }
    }

    // Main popup container
    Item {
        id: popupContainer
        width: Math.min(parent.width * 0.85, 480)
        anchors.centerIn: parent
        clip: true
        scale: root.visible ? 1 : 0.85
        opacity: root.visible ? 1 : 0.8

        // Calculate if scrolling is needed
        property bool scrollingNeeded: contentText.implicitHeight > (root.height * maxHeightRatio)
        property real targetHeight: scrollingNeeded ?
            (root.height * maxHeightRatio) :
            Math.min(contentText.implicitHeight + 110, root.height * maxHeightRatio)

        // Background panel
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.6
            radius: 20
            border.color: Theme.popupBorder
            border.width: 2
        }

        // Column layout for header, content and footer
        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // Header with title (optional)
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                // Add title here if needed
            }

            // Content area - dynamically switches between scrollable and non-scrollable
            Item {
                id: contentArea
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 8

                // Text content (visible when NO scrolling needed)
                AppText {
                    id: contentText
                    text: root.text
                    color: Theme.textColor
                    width: parent.width
                    visible: !popupContainer.scrollingNeeded
                }

                // ScrollView wrapper (visible ONLY when scrolling is needed)
                // IMPORTANT: ScrollBar customization moved to separate component
                QC.ScrollView {
                    id: scrollView
                    anchors.fill: parent
                    visible: popupContainer.scrollingNeeded

                    // Content dimensions
                    contentWidth: scrollableText.width
                    contentHeight: scrollableText.implicitHeight

                    // Scrollable text content
                    AppText {
                        id: scrollableText
                        text: root.text
                        color: Theme.textColor
                        width: contentArea.width
                    }
                }
            }

            // Footer with close button
            Button {
                text: qsTr("Закрыть")
                Layout.preferredWidth: parent.width * 0.85
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.bottomMargin: 10
                Layout.topMargin: 10
                onClicked: root.close()
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

    // Root animations
    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutCubic
            duration: 100
        }
    }

    // Public API
    function open(text) {
        root.text = text
        root.visible = true
        // Force height recalculation
        popupContainer.height = Qt.binding(function() { return popupContainer.targetHeight })
    }

    function close() {
        root.visible = false
    }
}
