// TrainingBar.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype TrainingBar
    \brief The footer of the training tab with difficulty buttons or a flip button.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    TrainingBar is the footer shown at the bottom of the training tab. Its
    content depends on the state of \l {Revise.StudyService::flipped}{Revise.StudyService.flipped}:

    \list
    \li When the card is \b not flipped, the bar shows a single
        \e {Flip} button that reveals the answer side of the card.
    \li When the card \b is flipped, the bar shows four difficulty buttons
        (\e Bad, \e Hard, \e Normal, \e Easy), each of which emits the
        corresponding signal so the caller can grade the answer.
    \endlist

    The component does not communicate with \l Revise.StudyService directly
    except for reading \c flipped and calling \c flip(). Grading is left to
    the caller through the four signals, so the same bar can be reused with
    different training logic.

    \section1 Example Usage

    \qml
    Revise.TrainingBar {
        onEasyClicked:   Revise.StudyService.grade(Revise.StudyService.Easy)
        onNormalClicked: Revise.StudyService.grade(Revise.StudyService.Normal)
        onHardClicked:   Revise.StudyService.grade(Revise.StudyService.Hard)
        onBadClicked:    Revise.StudyService.grade(Revise.StudyService.Bad)
    }
    \endqml
*/
Item {
    id: root
    width: parent.width
    height: 75

    /*!
        \qmlproperty int TrainingBar::currentIndex
        \brief The index of the current card in the training session.

        The property is provided for the caller to track the card being
        graded. TrainingBar itself does not use it for layout or behaviour.

        The default value is \c 0.
    */
    property int currentIndex: 0

    /*!
        \qmlsignal TrainingBar::easyClicked()
        \brief Emitted when the user presses the \e {Easy} difficulty button.
    */
    signal easyClicked()

    /*!
        \qmlsignal TrainingBar::normalClicked()
        \brief Emitted when the user presses the \e {Normal} difficulty button.
    */
    signal normalClicked()

    /*!
        \qmlsignal TrainingBar::hardClicked()
        \brief Emitted when the user presses the \e {Hard} difficulty button.
    */
    signal hardClicked()

    /*!
        \qmlsignal TrainingBar::badClicked()
        \brief Emitted when the user presses the \e {Bad} difficulty button.
    */
    signal badClicked()

    // Flipped
    RowLayout {
        visible: Revise.StudyService.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Revise.Button {
            text: qsTr("Плохо")
            color: Revise.Theme.trainingBarWrong
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            Layout.preferredHeight: 42
            onClicked: root.badClicked()
        }

        Revise.Button {
            text: qsTr("Трудно")
            color: Revise.Theme.trainingBarHard
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            Layout.preferredHeight: 42
            onClicked: root.hardClicked()
        }

        Revise.Button {
            text: qsTr("Сойдет")
            color: Revise.Theme.trainingBarNormal
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            Layout.preferredHeight: 42
            onClicked: root.normalClicked()
        }

        Revise.Button {
            text: qsTr("Легко")
            color: Revise.Theme.trainingBarEasy
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            font.bold: true
            Layout.preferredWidth: parent.width / 4 - 10
            Layout.preferredHeight: 42
            onClicked: root.easyClicked()
        }
    }

    // Not reversed
    RowLayout {
        visible: !Revise.StudyService.flipped
        anchors.margins: 12
        anchors.fill: parent
        clip: true

        Revise.Button {
            text: qsTr("Перевернуть")
            backgroundItem.color: "white"
            backgroundItem.opacity: 0.05
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            onClicked: Revise.StudyService.flip()
            font.bold: true
        }
    }
}