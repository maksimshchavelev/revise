// Training page

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts
import "../components"
import "../controls"
import "../views/training"
import "../dialogs"
import "qrc:/ui/theme"

Item {
    id: root

    signal trainingFinished()
    signal aborted();

    Shortcut {
        sequence: "Escape"
        onActivated: exitClicked()
    }

    function exitClicked() {
        studyService.pause()
        abortQuestionPopup.open(qsTr("Прервать тренировку? Прогресс не засчитается"))
    }

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
            visible: !abortQuestionPopup.visible
            onLoadingChanged: {
                if (loading) {
                    studyService.pause()
                } else {
                    studyService.resume()
                }
            }

            function get_html() {
                return root.flipped ? (countLines(studyService.cardText) > 2 ? verticalCenterText(studyService.cardText) : centerHtml(studyService.cardText))
                                    : centerHtml(studyService.cardText)
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

    QuestionPopup {
        id: abortQuestionPopup

        onAcceptClicked: {
            studyService.abort()
            root.aborted()
        }

        onDeclineClicked: {
            studyService.resume()
        }
    }
}
