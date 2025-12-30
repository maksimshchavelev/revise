// Footer for the training tab. Four buttons with difficulty levels

import QtQuick

import QtQuick.Layouts
import "qrc:/ui/theme"

Item {
    id: root
    width: parent.width
    height: 75

    property int currentIndex: 0
    signal easyClicked()
    signal normalClicked()
    signal hardClicked()
    signal badClicked()

    Rectangle {
        anchors.fill: parent
        color: Theme.toolbarColor
        opacity: 0.4
    }

    // Flipped
    RowLayout {
        visible: studyController.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Button {
            text: qsTr("Плохо")
            textColor: Theme.trainingBarWrong
            textBold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: {
                root.badClicked()
            }
        }

        Button {
            text: qsTr("Трудно")
            textColor: Theme.trainingBarHard
            textBold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: {
                root.hardClicked()
            }
        }

        Button {
            text: qsTr("Сойдет")
            textColor: Theme.trainingBarNormal
            textBold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: {
                root.normalClicked()
            }
        }

        Button {
            text: qsTr("Легко")
            textColor: Theme.trainingBarEasy
            textBold: true
            Layout.preferredWidth: parent.width / 4 - 10
            onClicked: {
                root.easyClicked()
            }
        }
    }

    // Not reversed
    RowLayout {
        visible: !studyController.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Button {
            text: qsTr("Перевернуть")
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            onClicked: studyController.flip()
            textBold: true
        }
    }
}

