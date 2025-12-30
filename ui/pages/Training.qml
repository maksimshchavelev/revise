// Training page

import QtQuick
import QtQuick.Layouts
import "../components"
import "qrc:/ui/theme"

Item {
    id: root

    signal trainingFinished()

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.2
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0  // Remove spacing between items

        // Progress bar with consistent layout space
        Item {
            id: progressBarContainer
            Layout.fillWidth: true
            Layout.preferredHeight: 30  // Fixed height for progress bar area
            Layout.topMargin: 8

            ColorfulProgressbar {
                id: progressBar
                anchors.centerIn: parent
                width: parent.width * 0.95
                height: 20
                visible: studyController.timeLimit > 0 && !studyController.flipped
                progress: studyController.timeLimit > 0 ? studyController.timeRemaining / studyController.timeLimit : 0
            }
        }

        Flickable {
            id: flickable
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 8
            contentWidth: width
            contentHeight: textContainer.height
            clip: true

            boundsBehavior: Flickable.StopAtBounds

            // Container for text to ensure proper centering
            Item {
                id: textContainer
                width: flickable.width
                height: Math.max(text.implicitHeight, flickable.height)

                AppText {
                    id: text
                    width: parent.width
                    font.pixelSize: Theme.textSizeMedium
                    wrapMode: Text.WordWrap
                    text: studyController.cardText

                    // Center horizontally and vertically
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    // Update position when text changes
                    onTextChanged: {
                        // Force layout update
                        if (text.implicitHeight < flickable.height) {
                            textContainer.height = flickable.height
                        } else {
                            textContainer.height = text.implicitHeight
                        }
                    }
                }
            }

            // Center content when text is smaller than flickable
            onHeightChanged: centerContent()
            Component.onCompleted: centerContent()

            function centerContent() {
                if (text.implicitHeight < height) {
                    contentY = (height - text.implicitHeight) / 2
                } else {
                    contentY = 0
                }
            }
        }

        TrainingBar {
            id: trainingBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            Layout.topMargin: 8
            onEasyClicked: studyController.next_card(0);
            onNormalClicked: studyController.next_card(1.5);
            onHardClicked: studyController.next_card(3);
            onBadClicked: studyController.next_card(5);
        }
    }

    Connections {
        target: studyController

        function onTraining_finished() {
            console.log("Training finished")
            root.trainingFinished()
        }
    }
}
