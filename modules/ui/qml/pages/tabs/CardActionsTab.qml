// CardActionsTab.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype CardActionsTab
    \brief A tab providing bulk operations for cards in a deck.
    \inqmlmodule Revise
    \inherits Revise.Flickable
    \since 1.0

    CardActionsTab provides a scrollable interface for performing bulk actions
    on cards within a deck. It is designed to be used inside a \l StackLayout
    within the deck editor.

    The tab allows the user to:
    \list
    \li Select all cards or clear the current selection (respecting the active search filter).
    \li Set a specific difficulty for the selected cards (or all cards if none are selected).
    \li Change the learning state of the selected cards.
    \li Force the selected cards to be reviewed today without resetting their difficulty.
    \endlist

    The component reads data directly from \l Revise.CardsModel and \l Revise.CardSearchModel
    to determine if actions should apply to all cards, found cards, or only selected ones.
    Actions that require a confirmation dialog in the parent component emit signals,
    while the "Review today" action is applied immediately via \l Revise.DeckService.

    \section1 Example Usage

    \qml
    Revise.CardActionsTab {
        id: actionsTab

        onSetDifficultyClicked: (difficulty) => confirmSetDifficulty(difficulty)
        onSetStateClicked: confirmSetState(actionsTab.cardStateComboBox.currentIndex)
    }
    \endqml
*/
Revise.Flickable {
    id: root

    /*!
        \qmlproperty Revise.ComboBox CardActionsTab::cardStateComboBox
        \brief The combo box used to select the target state for cards.

        Exposed as an alias so that the parent component can read the currently
        selected state when handling the \l setStateClicked signal.
    */
    property alias cardStateComboBox: cardStateComboBox

    /*!
        \qmlsignal CardActionsTab::setDifficultyClicked(real difficulty)
        \brief Emitted when the user clicks the button to apply the selected difficulty.

        The \a difficulty argument contains the value chosen via the internal slider.
        The parent component is expected to handle the confirmation and apply the change.
    */
    signal setDifficultyClicked(real difficulty)

    /*!
        \qmlsignal CardActionsTab::setStateClicked()
        \brief Emitted when the user clicks the button to apply the selected state.

        The parent component is expected to handle the confirmation and apply the change.
        The target state can be retrieved from \l cardStateComboBox.
    */
    signal setStateClicked()

    Layout.fillWidth: true
    Layout.fillHeight: true

    contentWidth: width
    contentHeight: cardActions.implicitHeight

    Column {
        id: cardActions
        width: parent.width

        Revise.SettingsPanel {
            width: parent.width
            header.label.text: qsTr("Пакетные действия")
            header.icon.source: "qrc:/res/img/action.svg"

            Revise.HorizontalDivider {
                width: parent.width
            }

            // Select all cards or deselect selected
            Revise.Button {
                width: parent.width
                clickable: Revise.CardsModel.cardsCount > 0

                text: {
                    if (Revise.CardsModel.selectedCount === 0) {
                        if (Revise.CardSearchModel.searchFront.length === 0)
                            return qsTr("Выбрать всё")
                        return qsTr("Выбрать найденное")
                    }
                    return qsTr("Снять выделение")
                }

                Revise.HoverableTooltip {
                    anchors.fill: parent
                    text: {
                        if (Revise.CardsModel.selectedCount === 0) {
                            if (Revise.CardSearchModel.searchFront.length === 0)
                                return qsTr("Выбрать все карточки в колоде")
                            return qsTr("Выбрать только найденные карточки")
                        }
                        return qsTr("Снять выделение со всех карточек в колоде")
                    }
                }

                onClicked: {
                    if (Revise.CardsModel.selectedCount === 0) {
                        if (Revise.CardSearchModel.searchFront.length === 0)
                            Revise.CardsModel.selectAll()
                        else
                            Revise.CardSearchModel.selectFoundCards()
                    } else {
                        Revise.CardsModel.deselectAll()
                    }
                }
            }

            Revise.HorizontalDivider {
                width: parent.width
            }

            // Set cards difficulty
            Revise.Text {
                width: parent.width
                text: qsTr("Задать сложность карточек")
                font.bold: true
            }

            Revise.ValueSlider {
                id: setDifficultySlider

                width: parent.width

                label.text: value.toFixed(1)
                slider.from: 0.0
                slider.to: 5.0
                slider.stepSize: 0.1
                value: 2.5
            }

            Revise.Button {
                width: parent.width
                clickable: Revise.CardsModel.cardsCount > 0

                text: {
                    if (Revise.CardsModel.selectedCount === 0)
                        return qsTr("Задать сложность")
                    return qsTr(`Задать сложность (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                }

                Revise.HoverableTooltip {
                    anchors.fill: parent
                    text: {
                        if (Revise.CardsModel.selectedCount === 0)
                            return qsTr("Выбранная сложность будет установлена для всех карточек")
                        qsTr("Выбранная сложность будет установлена для выбранных карточек")
                    }
                }

                onClicked: root.setDifficultyClicked(setDifficultySlider.value)
            }

            Revise.HorizontalDivider {
                width: parent.width
            }

            // Set cards status
            Revise.Text {
                width: parent.width
                text: qsTr("Задать статус карточек")
                font.bold: true
            }

            Revise.ComboBox {
                id: cardStateComboBox

                model: ListModel {
                    Component.onCompleted: {
                        append({
                            label: qsTr("Новая"),
                            textColor: Revise.Theme.green
                        })
                        append({
                            label: qsTr("Повторяемая"),
                            textColor: Revise.Theme.blue
                        })
                        append({
                            label: qsTr("Ошибочная"),
                            textColor: Revise.Theme.red
                        })
                    }
                }
            }

            Revise.Button {
                width: parent.width
                clickable: Revise.CardsModel.cardsCount > 0

                text: {
                    if (Revise.CardsModel.selectedCount === 0)
                        return qsTr("Задать статус")
                    return qsTr(`Задать статус (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                }

                Revise.HoverableTooltip {
                    anchors.fill: parent
                    text: {
                        if (Revise.CardsModel.selectedCount === 0)
                            return qsTr("Выбранный статус будет установлен для всех карточек")
                        qsTr("Выбранный статус будет установлен для выбранных карточек")
                    }
                }

                onClicked: root.setStateClicked()
            }

            Revise.HorizontalDivider {
                width: parent.width
            }

            Revise.Text {
                width: parent.width
                text: qsTr("Повторить карточки сегодня")
                font.bold: true
            }

            // Review today
            Revise.Button {
                width: parent.width
                clickable: Revise.CardsModel.cardsCount > 0

                text: {
                    if (Revise.CardsModel.selectedCount === 0)
                        return qsTr("Повторить сегодня")
                    return qsTr(`Повторить сегодня (${Revise.CardsModel.selectedCount} / ${Revise.CardsModel.cardsCount})`)
                }

                Revise.HoverableTooltip {
                    anchors.fill: parent
                    text: {
                        if (Revise.CardsModel.selectedCount === 0)
                            return qsTr("Все карточки будут повторены сегодня без сброса сложности")
                        qsTr("Только выбранные карточки будут повторены сегодня без сброса сложности")
                    }
                }

                onClicked: {
                    let draft = new Revise.updateCardDraft();
                    draft.nextReview = new Date();

                    if (Revise.CardsModel.selectedCount === 0) {
                        Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
                    } else {
                        Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
                    }
                }
            }
        }
    }
}
