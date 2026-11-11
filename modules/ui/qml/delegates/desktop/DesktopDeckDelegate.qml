// DeckDelegate.qml
// A deck that will be displayed on the main page

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype DeckDelegate
    \brief A visual representation of a deck on the main page.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    DeckDelegate displays information about a specific deck, including its name,
    description, card counts (new, review, incorrect), and time limit. It provides
    actions to start studying, edit, export, or remove the deck through the integrated
    kebab menu.

    The component includes hover animations and tooltips for truncated text to
    improve the user experience. The "Study" button is automatically disabled if
    there are no cards available to review today.

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

    width: 170
    height: 220

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
        signal
    studyClicked

    /*!
        \qmlsignal DeckDelegate::editClicked()
        \brief Emitted when the "Edit" option is selected from the kebab menu.
    */
        signal
    editClicked

    /*!
        \qmlsignal DeckDelegate::removeClicked()
        \brief Emitted when the "Remove" option is selected from the kebab menu.
    */
        signal
    removeClicked

    /*!
        \qmlsignal DeckDelegate::exportClicked()
        \brief Emitted when the "Export" option is selected from the kebab menu.
    */
        signal
    exportClicked

    Item {
        anchors.fill: parent

        scale: hoverHandler.hovered ? 1.02 : 1.0

        Rectangle {
            id: tint
            anchors.fill: parent
            color: Revise.Theme.surfaceColor
            radius: 10
            layer.effect: Revise.Shadow
            {
            }
        }

        ColumnLayout {
            id: content

            anchors {
                fill: parent
                margins: 8
            }

            // Deck name
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Revise.Text {
                    id: nameText
                    height: parent.height
                    width: menuButton.x - parent.x - 2
                    text: root.deckNameProperty
                    elide: Text.ElideRight
                    color: Revise.Theme.foregroundColor
                    font.bold: true
                    wrapMode: Revise.Text.Wrap

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: root.deckNameProperty
                    }
                }

                Revise.IconButton {
                    id: menuButton

                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 2

                    icon.source: "qrc:/res/img/menu.svg"
                    size: 24
                    background.color: "transparent"
                    opacity: 0.7

                    onClicked: {
                        kekabMenu.x = menuButton.x + menuButton.width + 10
                        kekabMenu.y = menuButton.y
                        kekabMenu.clampPosition()
                        kekabMenu.open()
                    }
                }
            }

            // Deck description
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Revise.Text {
                    anchors.fill: parent
                    text: root.deckDescriptionProperty.length > 0 ? root.deckDescriptionProperty : qsTr("Без описания")
                    elide: Text.ElideRight
                    verticalAlignment: Revise.Text.AlignVCenter
                    color: Revise.Theme.secondaryForegroundColor
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    wrapMode: Revise.Text.Wrap
                }

                Revise.HoverableTooltip {
                    visible: root.deckDescriptionProperty.length > 0
                    anchors.fill: parent
                    text: root.deckDescriptionProperty
                }
            }

            // New cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Новые")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.green
                }

                Revise.HorizontalSpacer {
                }

                Revise.Text {
                    text: root.newCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.newCardsProperty > 0 ? Revise.Theme.green : Revise.Theme.secondaryForegroundColor
                }
            }

            // Review cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Закрепляемые")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.blue
                }

                Revise.HorizontalSpacer {
                }

                Revise.Text {
                    text: root.reviewCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.reviewCardsProperty > 0 ? Revise.Theme.blue : Revise.Theme.secondaryForegroundColor
                }
            }

            // Incorrect cards
            RowLayout {
                Layout.fillWidth: true

                Revise.Text {
                    text: qsTr("Ошибочные")
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.red
                }

                Revise.HorizontalSpacer {
                }

                Revise.Text {
                    text: root.incorrectCardsProperty
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: root.incorrectCardsProperty > 0 ? Revise.Theme.red : Revise.Theme.secondaryForegroundColor
                }
            }

            // Time limit
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                property bool timeUnlimited: root.timeLimitProperty === 0
                property int iconSize: 16

                Revise.Icon {
                    size: parent.iconSize
                    source: "qrc:/res/img/clocks.svg"
                    color: Revise.Theme.foregroundColor
                }

                Revise.HorizontalSpacer {
                }

                Revise.Icon {
                    visible: parent.timeUnlimited
                    size: parent.iconSize
                    source: "qrc:/res/img/infinity.svg"
                    color: Revise.Theme.secondaryForegroundColor
                }

                Revise.Text {
                    visible: !parent.timeUnlimited
                    text: qsTr(`${root.timeLimitProperty} с`)
                    font.pixelSize: Revise.Theme.fontSizeSmall
                    color: Revise.Theme.foregroundColor
                    verticalAlignment: Revise.Text.AlignVCenter
                }
            }

            Revise.Spacer {
            }

            Revise.Button {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: parent.width * 0.85
                Layout.preferredHeight: 30

                text: "Учить"
                font.bold: true
                clickable: root.repeatableTodayProperty

                onClicked: root.studyClicked()

                Revise.HoverableTooltip {
                    visible: !root.repeatableTodayProperty
                    anchors.fill: parent
                    text: qsTr("Сегодня учить нечего. Отдохните")
                }
            }

            Revise.Spacer {
            }
        }

        Behavior on scale {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 60
            }
        }
    }

    Revise.KebabMenu {
        id: kekabMenu

        Revise.KebabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Экспортировать")
            onClicked: {
                kekabMenu.close()
                root.exportClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Удалить")
            color: Revise.Theme.red
            onClicked: {
                kekabMenu.close()
                root.removeClicked()
            }
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
