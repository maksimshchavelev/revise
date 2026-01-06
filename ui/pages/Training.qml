// Training page

import QtQuick
import QtQuick.Layouts
import "../components"
import "../controls"
import "../views/training"
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
                visible: studyService.timeLimit > 0 && !studyService.flipped
                progress: studyService.timeLimit > 0 ? studyService.timeRemaining / studyService.timeLimit : 0
            }
        }

        Flickable {
            id: flickable
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 8
            contentWidth: width
            contentHeight: Math.max(text.implicitHeight, height)
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: htmlHelper.has_mathjax(text) ?
                                    Flickable.HorizontalFlick :
                                    Flickable.VerticalFlick

            AppText {
                id: text
                width: parent.width
                anchors.centerIn: parent
                font.pixelSize: Theme.textSizeMedium
                wrapMode: htmlHelper.has_mathjax(text) ? Text.NoWrap : Text.WordWrap
                text: {
                    var prepared_text = htmlHelper.scale_images_to_width(studyService.cardText, parent.width)
                    prepared_text = htmlHelper.replace_mathjax_to_images(prepared_text,
                                                                         Theme.textColor,
                                                                         Theme.textSizeMedium)
                }

                textFormat: Text.RichText
                horizontalAlignment: htmlHelper.has_mathjax(text) ? Text.AlignLeft : Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        TrainingBar {
            id: trainingBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            Layout.topMargin: 8
            onEasyClicked: studyService.next_card(0);
            onNormalClicked: studyService.next_card(1.35);
            onHardClicked: studyService.next_card(3);
            onBadClicked: studyService.next_card(5);
        }
    }

    Connections {
        target: studyService

        function onTraining_finished() {
            root.trainingFinished()
        }
    }
}
