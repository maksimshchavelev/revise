// Training page

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property bool openedAsWindow: false
    property var pageParams: null // nothing now

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Progress bar with consistent layout space
        Item {
            id: progressBarContainer
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            Layout.topMargin: 8

            Revise.ColorfulProgressbar {
                id: progressBar
                anchors.centerIn: parent
                width: parent.width * 0.95
                height: 20
                visible: studyService.timeLimit > 0 && !studyService.flipped
                progress: studyService.timeLimit > 0 ? studyService.timeRemaining / studyService.timeLimit : 0
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !htmlCard.visible
        }

        Revise.CardView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: studyService.flipped ? studyService.back : studyService.front
            onLoadingChanged: {
                if (loading) {
                    studyService.pause()
                } else {
                    studyService.resume()
                }
            }
        }

        Revise.TrainingBar {
            id: trainingBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            Layout.topMargin: 8
            onEasyClicked: studyService.answer(0.0)
            onNormalClicked: studyService.answer(1.35)
            onHardClicked: studyService.answer(3.0)
            onBadClicked: studyService.answer(5.0)
        }
    }
}
