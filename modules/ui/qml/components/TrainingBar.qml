// Footer for the training tab. Four buttons with difficulty levels

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root
    width: parent.width
    height: 75

    property int currentIndex: 0
    signal easyClicked()
    signal normalClicked()
    signal hardClicked()
    signal badClicked()

    // Flipped
    RowLayout {
        visible: studyService.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Revise.Button {
            text: qsTr("Плохо")
            color: Revise.Theme.trainingBarWrong
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: root.badClicked()
        }

        Revise.Button {
            text: qsTr("Трудно")
            color: Revise.Theme.trainingBarHard
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: root.hardClicked()
        }

        Revise.Button {
            text: qsTr("Сойдет")
            color: Revise.Theme.trainingBarNormal
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: root.normalClicked()
        }

        Revise.Button {
            text: qsTr("Легко")
            color: Revise.Theme.trainingBarEasy
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: root.easyClicked()
        }
    }

    // Not reversed
    RowLayout {
        visible: !studyService.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Revise.Button {
            text: qsTr("Перевернуть")
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            onClicked: studyService.flip()
            font.bold: true
        }
    }
}

