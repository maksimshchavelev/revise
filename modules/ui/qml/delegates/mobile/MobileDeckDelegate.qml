// DeckDelegate.qml
// A mobile version of a deck that will be displayed on the main page

import QtQuick
import Revise as Revise

/*!
    \qmltype DeckDelegate
    \brief A mobile representation of a deck on the main page.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    DeckDelegate displays information about a specific deck on mobile devices.
    It features a horizontally stretched layout containing the deck name,
    card statistics (new, review, incorrect), time limit, description, and a
    primary action button to start studying.

    The component also includes a lazily loaded kebab menu for secondary actions
    such as editing, exporting, or removing the deck. The "Study" button is
    automatically disabled if \l repeatableTodayProperty is \c false.

    \section1 Example Usage

    \qml
    Revise.DeckDelegate {
        deckNameProperty: "My Deck"
        deckDescriptionProperty: "A deck for learning"
        newCardsProperty: 10
        reviewCardsProperty: 5
        incorrectCardsProperty: 2
        timeLimitProperty: 60
        deckIdProperty: 1
        repeatableTodayProperty: true

        onStudyClicked: startStudySession()
        onEditClicked: openEditor()
        onRemoveClicked: deleteDeck()
        onExportClicked: exportDeck()
    }
    \endqml
*/
Item {
    id: root

    width: parent.width
    height: 140

    /*!
        \qmlproperty int DeckDelegate::newCardsProperty
        \brief The number of cards that have not yet been studied.
    */
    property int newCardsProperty

    /*!
        \qmlproperty int DeckDelegate::reviewCardsProperty
        \brief The number of cards that have already been studied and need to be reviewed.
    */
    property int reviewCardsProperty

    /*!
        \qmlproperty int DeckDelegate::incorrectCardsProperty
        \brief The number of cards that the user answered incorrectly during the last study session.
    */
    property int incorrectCardsProperty

    /*!
        \qmlproperty int DeckDelegate::deckIdProperty
        \brief The unique identifier of the deck.
    */
    property int deckIdProperty

    /*!
        \qmlproperty int DeckDelegate::timeLimitProperty
        \brief The time limit for answering a card, in seconds.

        A value of \c 0 indicates no time limit.
    */
    property int timeLimitProperty

    /*!
        \qmlproperty string DeckDelegate::deckNameProperty
        \brief The name of the deck.
    */
    property string deckNameProperty

    /*!
        \qmlproperty string DeckDelegate::deckDescriptionProperty
        \brief The description of the deck.
    */
    property string deckDescriptionProperty

    /*!
        \qmlproperty bool DeckDelegate::repeatableTodayProperty
        \brief Whether the deck has cards available for review today.
    */
    property bool repeatableTodayProperty

    /*!
        \qmlsignal DeckDelegate::studyClicked()
        \brief Emitted when the "Study" button is clicked.
    */
        signal studyClicked

    /*!
        \qmlsignal DeckDelegate::editClicked()
        \brief Emitted when the "Edit" option is selected from the kebab menu.
    */
        signal editClicked

    /*!
        \qmlsignal DeckDelegate::removeClicked()
        \brief Emitted when the "Remove" option is selected from the kebab menu.
    */
        signal removeClicked

    /*!
        \qmlsignal DeckDelegate::exportClicked()
        \brief Emitted when the "Export" option is selected from the kebab menu.
    */
        signal exportClicked

    Rectangle {
        anchors.fill: parent
        color: Revise.Theme.surfaceColor
        radius: 10
    }

    Column {
        id: content
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Item {
            width: parent.width
            height: Math.max(nameText.implicitHeight, menuButton.height)

            Revise.Text {
                id: nameText
                width: parent.width - menuButton.width - 12
                text: root.deckNameProperty
                elide: Text.ElideRight
                maximumLineCount: 2
                color: Revise.Theme.foregroundColor
                verticalAlignment: Revise.Text.AlignVCenter
                font.bold: true
                wrapMode: Revise.Text.Wrap
            }

            Revise.IconButton {
                id: menuButton
                width: 24
                height: 24
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "qrc:/res/img/menu.svg"
                background.color: "transparent"
                opacity: 0.7

                onClicked: {
                    kebabMenu.x = menuButton.x;
                    kebabMenu.y = menuButton.y;
                    kebabMenu.clampPosition();
                    kebabMenu.open();
                }
            }
        }

        Item {
            id: statsBlock
            width: parent.width
            height: parent.height - content.spacing - nameText.parent.height

            Item {
                id: statsColumn
                width: 150
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left

                Column {
                    id: statsList
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 6

                    Revise.KeyValue {
                        width: parent.width
                        key.text: qsTr("Новые")
                        key.color: Revise.Theme.green
                        key.font.pixelSize: Revise.Theme.fontSizeSmall
                        value.text: root.newCardsProperty
                        value.color: root.newCardsProperty > 0 ? Revise.Theme.green : Revise.Theme.secondaryForegroundColor
                        value.font.pixelSize: Revise.Theme.fontSizeSmall
                    }

                    Revise.KeyValue {
                        width: parent.width
                        key.text: qsTr("Закрепляемые")
                        key.color: Revise.Theme.blue
                        key.font.pixelSize: Revise.Theme.fontSizeSmall
                        value.text: root.reviewCardsProperty
                        value.color: root.reviewCardsProperty > 0 ? Revise.Theme.blue : Revise.Theme.secondaryForegroundColor
                        value.font.pixelSize: Revise.Theme.fontSizeSmall
                    }

                    Revise.KeyValue {
                        width: parent.width
                        key.text: qsTr("Ошибочные")
                        key.color: Revise.Theme.red
                        key.font.pixelSize: Revise.Theme.fontSizeSmall
                        value.text: root.incorrectCardsProperty
                        value.color: root.incorrectCardsProperty > 0 ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                        value.font.pixelSize: Revise.Theme.fontSizeSmall
                    }
                }

                Item {
                    id: timeLimitRow
                    width: parent.width
                    height: 20
                    anchors.bottom: parent.bottom

                    property bool timeUnlimited: root.timeLimitProperty === 0
                    property int iconSize: 16

                    Revise.Icon {
                        id: clockIcon
                        width: parent.iconSize
                        height: parent.iconSize
                        source: "qrc:/res/img/clocks.svg"
                        color: Revise.Theme.foregroundColor
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Revise.Icon {
                        id: infinityIcon
                        width: parent.iconSize
                        height: parent.iconSize
                        source: "qrc:/res/img/infinity.svg"
                        color: Revise.Theme.secondaryForegroundColor
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        visible: parent.timeUnlimited
                    }

                    Revise.Text {
                        text: qsTr(`${root.timeLimitProperty} с`)
                        font.pixelSize: Revise.Theme.fontSizeSmall
                        color: Revise.Theme.foregroundColor
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        visible: !parent.timeUnlimited
                    }
                }
            }

            Item {
                id: descriptionBlock
                anchors.left: statsColumn.right
                anchors.leftMargin: 12
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                Revise.Text {
                    id: descriptionText
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: studyButton.top
                    text: root.deckDescriptionProperty.length > 0 ? root.deckDescriptionProperty : qsTr("Без описания")
                    elide: Text.ElideRight
                    color: Revise.Theme.secondaryForegroundColor
                    maximumLineCount: 3
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    wrapMode: Revise.Text.Wrap
                }

                Revise.Button {
                    id: studyButton
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 30

                    text: "Учить"
                    font.bold: true
                    clickable: root.repeatableTodayProperty

                    onClicked: root.studyClicked()
                }
            }
        }
    }

    Revise.KebabMenu {
        id: kebabMenu

        Revise.KebabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kebabMenu.close()
                root.editClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Экспортировать")
            onClicked: {
                kebabMenu.close()
                root.exportClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Удалить")
            color: Revise.Theme.red
            onClicked: {
                kebabMenu.close()
                root.removeClicked()
            }
        }
    }
}
