// DeckPropertiesTab.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype DeckPropertiesTab
    \brief A tab containing deck properties, study limits, and bulk actions.
    \inqmlmodule Revise
    \inherits ColumnLayout
    \since 1.0

    DeckPropertiesTab provides a comprehensive interface for editing a deck's
    core information, configuring study limits, and performing bulk actions
    on its cards. It is designed to be used inside a \l StackLayout within the
    deck editor.

    The component consists of three main panels:
    \list
    \li \b {Main parameters} — for editing the deck's name and description.
    \li \b {Limits} — for setting the time limit, as well as limits on new,
        review, and incorrect cards.
    \li \b {Deck actions} — for restarting the learning process or removing
        the deck entirely.
    \endlist

    A save button is pinned to the bottom of the layout.

    \section1 Example Usage

    \qml
    Revise.DeckPropertiesTab {
        deckName.textField.text: deck.name
        deckDescription.textField.text: deck.description

        onSaveClicked: saveDeckChanges()
        onRemoveDeckClicked: deleteDeck()
        onRestartLearningClicked: resetDeckProgress()
        onRemoveCardsClicked: deleteCards()
    }
    \endqml
*/
ColumnLayout {
    id: root

    /*!
        \qmlproperty Revise.FormTextField DeckPropertiesTab::deckName
        \brief The input field for the deck's name.
    */
    property alias deckName: deckName

    /*!
        \qmlproperty Revise.FormTextField DeckPropertiesTab::deckDescription
        \brief The input field for the deck's description.
    */
    property alias deckDescription: deckDescription

    /*!
        \qmlproperty Revise.Switch DeckPropertiesTab::limitTime
        \brief The switch that enables or disables the time limit.
    */
    property alias limitTime: limitTime

    /*!
        \qmlproperty Revise.SpinBox DeckPropertiesTab::deckTimeLimit
        \brief The spin box for configuring the answer time limit in seconds.
    */
    property alias deckTimeLimit: deckTimeLimit

    /*!
        \qmlproperty Revise.SpinBox DeckPropertiesTab::deckNewLimit
        \brief The spin box for configuring the maximum number of new cards.
    */
    property alias deckNewLimit: deckNewLimit

    /*!
        \qmlproperty Revise.SpinBox DeckPropertiesTab::deckReviewLimit
        \brief The spin box for configuring the maximum number of review cards.
    */
    property alias deckReviewLimit: deckReviewLimit

    /*!
        \qmlproperty Revise.SpinBox DeckPropertiesTab::deckIncorrectLimit
        \brief The spin box for configuring the maximum number of incorrect cards.
    */
    property alias deckIncorrectLimit: deckIncorrectLimit

    /*!
        \qmlsignal DeckPropertiesTab::restartLearningClicked()
        \brief Emitted when the "Restart learning" button is clicked.
    */
    signal restartLearningClicked()

    /*!
        \qmlsignal DeckPropertiesTab::removeDeckClicked()
        \brief Emitted when the "Remove deck" button is clicked.
    */
    signal removeDeckClicked()

    /*!
        \qmlsignal DeckPropertiesTab::saveClicked()
        \brief Emitted when the "Save" button is clicked.
    */
    signal saveClicked()

    /*!
        \qmlsignal DeckPropertiesTab::removeCardsClicked()
        \brief Emitted when the "Remove cards" button is clicked.
    */
    signal removeCardsClicked()

    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 8

    Revise.Flickable {
        Layout.fillWidth: true
        Layout.fillHeight: true

        contentWidth: width
        contentHeight: deckProperties.implicitHeight

        Column {
            id: deckProperties

            width: parent.width - 16
            spacing: 16

            // Panel showing deck primary options
            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/deck.svg"
                header.label.text: qsTr("Основные параметры")

                Revise.FormTextField {
                    id: deckName
                    width: parent.width
                    label.text: qsTr("Название колоды")
                    textField.placeholder.text: qsTr("Введите название колоды...")
                }

                Revise.FormTextField {
                    id: deckDescription
                    width: parent.width
                    label.text: qsTr("Описание колоды (необязательно)")
                    textField.placeholder.text: qsTr("Введите описание колоды...")
                }
            }

            // Panel showing deck limits
            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/restrictions.svg"
                header.label.text: qsTr("Ограничения")

                Revise.SettingsRow {
                    width: parent.width
                    text: qsTr("Ограничить время ответа")
                    control: Revise.Switch {
                        id: limitTime
                    }
                }

                Revise.HorizontalDivider {
                    width: parent.width
                }

                Revise.SettingsRow {
                    width: parent.width
                    text: qsTr("Время ответа, сек")

                    control: Revise.SpinBox {
                        id: deckTimeLimit

                        from: 0
                        to: 3600
                        editable: limitTime.checked

                        Revise.HoverableTooltip {
                            anchors.fill: parent
                            text: qsTr("От 0 до 3600")
                        }
                    }
                }

                Revise.SettingsRow {
                    width: parent.width
                    text: qsTr("Ограничение новых карточек")

                    control: Revise.SpinBox {
                        id: deckNewLimit

                        from: 1
                        to: 1000

                        Revise.HoverableTooltip {
                            anchors.fill: parent
                            text: qsTr("От 1 до 1000")
                        }
                    }
                }

                Revise.SettingsRow {
                    width: parent.width
                    text: qsTr("Ограничение повторяемых карточек")

                    control: Revise.SpinBox {
                        id: deckReviewLimit

                        from: 1
                        to: 1000

                        Revise.HoverableTooltip {
                            anchors.fill: parent
                            text: qsTr("От 1 до 1000")
                        }
                    }
                }

                Revise.SettingsRow {
                    width: parent.width
                    text: qsTr("Ограничение ошибочных карточек")

                    control: Revise.SpinBox {
                        id: deckIncorrectLimit

                        from: 1
                        to: 1000

                        Revise.HoverableTooltip {
                            anchors.fill: parent
                            text: qsTr("От 1 до 1000")
                        }
                    }
                }
            }

            //  A panel showing deck actions
            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/action.svg"
                header.label.text: qsTr("Действия с колодой")

                // Restart learning
                Revise.Button {
                    width: parent.width
                    clickable: Revise.CardsModel.cardsCount > 0

                    text: {
                        if (Revise.CardsModel.selectedCount === 0)
                            return qsTr("Начать обучение заново")
                        else return qsTr(`Учить заново (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                    }

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: {
                            if (Revise.CardsModel.selectedCount === 0)
                                return qsTr("Сложность и время следующего повтора будут сброшены для всех карточек")
                            else
                                return qsTr("Сложность и время следующего повтора будут сброшены для выбранных карточек")
                        }
                    }

                    onClicked: root.restartLearningClicked()
                }

                // Remove cards
                Revise.Button {
                    width: parent.width
                    color: Revise.Theme.red
                    clickable: Revise.CardsModel.cardsCount > 0

                    text: {
                        if (Revise.CardsModel.selectedCount === 0)
                            return qsTr("Удалить все карточки")
                        else return qsTr(`Удалить карточки (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                    }

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: {
                            if (Revise.CardsModel.selectedCount === 0)
                                return qsTr("Все карточки будут удалены. Действие необратимо")
                            else
                                return qsTr("Только выбранные карточки будут удалены. Действие необратимо")
                        }
                    }

                    onClicked: root.removeCardsClicked()
                }

                Revise.Button {
                    width: parent.width
                    text: qsTr("Удалить колоду")
                    color: Revise.Theme.red

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: qsTr("Колода будет удалена. Действие необратимо")
                    }

                    onClicked: root.removeDeckClicked()
                }
            }
        }
    }

    Revise.Button {
        Layout.fillWidth: true
        text: qsTr("Сохранить")
        icon.source: "qrc:/res/img/save.svg"
        backgroundItem.color: Revise.Theme.primaryColor
        onClicked: root.saveClicked()
    }
}
