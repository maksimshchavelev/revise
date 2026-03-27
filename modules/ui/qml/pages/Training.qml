// Training page

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts
import Revise

Item {
    id: root

    property var pageParams: null // nothing now

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    QQC.BusyIndicator {
        anchors.centerIn: parent
        width: parent.width * 0.35
        height: width
        running: htmlCard.loading
    }

    ColumnLayout {
        visible: !htmlCard.loading
        anchors.fill: parent
        spacing: 0

        // Progress bar with consistent layout space
        Item {
            id: progressBarContainer
            Layout.fillWidth: true
            Layout.preferredHeight: 30
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

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !htmlCard.visible
        }

        HtmlView {
            id: htmlCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            html: loading ? "" : get_html()
            onLoadingChanged: {
                if (loading) {
                    studyService.pause()
                } else {
                    studyService.resume()
                }
            }

            function get_html() {
                return studyService.flipped ? (countLines(studyService.back) > 2 ? verticalCenterText(studyService.back) : centerHtml(studyService.back))
                                    : centerHtml(studyService.front)
            }
        }

        TrainingBar {
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
