// Training page
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property bool openedAsWindow: false
    property string windowTitle: qsTr("Обучение - Revise")
    property var pageParams: null // nothing now

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: progressBarContainer
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 30
            Layout.margins: 12
            spacing: 12

            Revise.ColorfulProgressbar {
                id: progressBar
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                visible: studyService.timeLimit > 0 && !studyService.flipped
                progress: studyService.timeLimit > 0 ? studyService.timeRemaining
                                                       / studyService.timeLimit : 0
            }

            Revise.IconButton {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                source: "qrc:/res/img/edit.svg"
                color: Revise.Theme.textColor
                size: 24
                onClicked: {
                    studyService.pause()
                    router.navigate("cardEditor", {
                                        "editMode": true,
                                        "card": studyService.card
                                    }, Revise.page.Page)
                }
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

    function onEnter() {
        studyService.reloadCurrentCard()
        studyService.resume()
    }

    function onExit() {}
}
