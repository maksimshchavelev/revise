// Training page

import QtQuick
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

            Revise.IconButton {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                icon.source: "qrc:/res/img/back.svg"
                icon.color: Revise.Theme.foregroundColor
                background.color: "transparent"
                size: 32
                onClicked: {
                    Revise.StudyService.pause()
                    abortLearningDialog.requestOpen()
                }
            }

            Revise.ColorfulProgressbar {
                id: progressBar
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                visible: Revise.StudyService.timeLimit > 0 && !Revise.StudyService.flipped
                progress: Revise.StudyService.timeLimit > 0 ? Revise.StudyService.timeRemaining
                    / Revise.StudyService.timeLimit : 0
            }

            Revise.IconButton {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                icon.source: "qrc:/res/img/edit.svg"
                icon.color: Revise.Theme.foregroundColor
                background.color: "transparent"
                size: 32
                onClicked: {
                    Revise.StudyService.pause()
                    Revise.Router.navigate("cardEditor", {
                        "editMode": true,
                        "card": Revise.StudyService.card
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
            html: Revise.StudyService.flipped ? Revise.StudyService.back : Revise.StudyService.front
            onLoadingChanged: {
                if (loading) {
                    Revise.StudyService.pause()
                } else {
                    Revise.StudyService.resume()
                }
            }
        }

        Revise.TrainingBar {
            id: trainingBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            Layout.topMargin: 8
            onEasyClicked: Revise.StudyService.answer(0.0)
            onNormalClicked: Revise.StudyService.answer(1.5)
            onHardClicked: Revise.StudyService.answer(3.0)
            onBadClicked: Revise.StudyService.answer(5.0)
        }
    }

    Revise.ConfirmDialog {
        id: abortLearningDialog

        titleText: qsTr("Прервать обучение?")
        messageText: qsTr("Если выйти сейчас, прогресс этой сессии будет потерян")
        confirmButtonText: qsTr("Прервать")
        suppressed: !Revise.SettingsService.askAbortLearning

        onAccepted: {
            Revise.StudyService.abort()
            Revise.SettingsService.askAbortLearning = !dontAskAgain
            Revise.Router.navigate("home", {}, Revise.page.Page)
        }

        onRejected: Revise.StudyService.resume()
    }

    function onEnter() {
        Revise.StudyService.reloadCurrentCard()
        Revise.StudyService.resume()
    }

    function onExit() {
    }
}
