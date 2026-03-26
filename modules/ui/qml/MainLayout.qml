import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Revise

Item {
    id: root
    anchors.fill: parent

    signal deckClicked(int deckId, bool repeatableToday)
    signal addDeckClicked()

    property var pageParams: null

    // This function is called when the back button is pressed.
    // Ideally, it should be on every page.
    function exitClicked() {
        if (toolbar.currentIndex === 1) {
            toolbar.currentIndex = 0
        } else if (toolbar.currentIndex === 0 && !exitQuestionPopup.visible) {
            exitQuestionPopup.open(qsTr("Выйти?"))
        }
    }

    Item {
        id: headerItem
        width: parent.width
        height: 70
        anchors.top: parent.top

        Rectangle {
            anchors.fill: parent
            color: Theme.toolbarColor
            opacity: 0.4
        }
        
        StreakIndicator {
            streak: streakService.streak
            updatedToday: streakService.updatedToday
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.margins: 15
        }

        ToolButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: 15
            activeIcon: "qrc:/res/img/add_deck.png"
            width: 36
            height: 36
            showBackground: false
            contentMargins: 0

            onClicked: root.addDeckClicked()
        }
    }

    ToolBar {
        id: toolbar
        width: parent.width
        anchors.bottom: parent.bottom

        ToolButton {
            activeIcon: "qrc:/res/img/decks_selected.png"
            inactiveIcon: "qrc:/res/img/decks.png"
        }

        ToolButton {
            activeIcon: "qrc:/res/img/statistics_selected.png"
            inactiveIcon: "qrc:/res/img/statistics.png"
        }
    }

    StackLayout {
        id: pages
        anchors.top: headerItem.bottom
        anchors.bottom: toolbar.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: toolbar.currentIndex

        Home {
            onDeckClicked: function (deckId, repeatableToday) {
                root.deckClicked(deckId, repeatableToday)
            }
        }

        Statistics {}
    }

    QuestionPopup {
        id: exitQuestionPopup

        onAcceptClicked: Qt.exit(0)
    }
}
