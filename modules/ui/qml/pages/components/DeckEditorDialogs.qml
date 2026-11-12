// DeckEditorDialogs.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype DeckEditorDialogs
    \brief A collection of confirmation dialogs for deck and card operations.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    DeckEditorDialogs centralizes the confirmation dialogs used in the deck editor
    for destructive or bulk actions, such as removing cards, resetting learning progress,
    or changing properties for multiple cards at once.

    The component handles the logic for respecting the user's "don't ask again" preferences
    stored in \l Revise.SettingsService and applies the changes directly via \l Revise.DeckService
    when the action is confirmed.
*/
Item {
    id: root

    /*!
        \qmlmethod void DeckEditorDialogs::requestRemoveCardDialog(int cardId)
        \brief Requests the dialog to confirm the removal of a single card.

        The \a cardId parameter specifies the card to be removed.
    */
    function requestRemoveCardDialog(cardId) {
        removeCardDialog.cardId = cardId;
        removeCardDialog.requestOpen();
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestRemoveManyCardsDialog()
        \brief Requests the dialog to confirm the removal of multiple cards.

        The target cards are determined by the current selection in \l Revise.CardsModel.
    */
    function requestRemoveManyCardsDialog() {
        removeCardsDialog.requestOpen();
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestRelearnOneCardDialog(int cardId)
        \brief Requests the dialog to confirm resetting the learning progress of a single card.

        The \a cardId parameter specifies the card to be reset.
    */
    function requestRelearnOneCardDialog(cardId) {
        relearnCardDialog.cardId = cardId;
        relearnCardDialog.requestOpen();
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestRelearnManyCardsDialog()
        \brief Requests the dialog to confirm resetting the learning progress of multiple cards.

        The target cards are determined by the current selection in \l Revise.CardsModel.
    */
    function requestRelearnManyCardsDialog() {
        relearnCardsDialog.requestOpen()
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestRemoveDeckDialog(int deckId)
        \brief Requests the dialog to confirm the removal of a deck.

        The \a deckId parameter specifies the deck to be removed.
    */
    function requestRemoveDeckDialog(deckId) {
        removeDeckDialog.deckId = deckId;
        removeDeckDialog.requestOpen();
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestSetDifficultyForManyCardsDialog(real difficulty)
        \brief Requests the dialog to confirm setting the difficulty for multiple cards.

        The \a difficulty parameter specifies the new difficulty value to apply.
    */
    function requestSetDifficultyForManyCardsDialog(difficulty) {
        setDifficultyForCardsDialog.difficulty = difficulty;
        setDifficultyForCardsDialog.requestOpen();
    }

    /*!
        \qmlmethod void DeckEditorDialogs::requestSetStateForManyCardsDialog(string state)
        \brief Requests the dialog to confirm setting the state for multiple cards.

        The \a state parameter specifies the new state name to apply.
    */
    function requestSetStateForManyCardsDialog(state) {
        setStateForCardsDialog.state = state;
        setStateForCardsDialog.requestOpen();
    }

    // Remove one card?
    Revise.ConfirmDialog {
        id: removeCardDialog

        property int cardId

        titleText: qsTr("Удалить карточку?")
        messageText: qsTr("Карточка будет удалена без возможности восстановления")
        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveOneCard

        onAccepted: {
            Revise.SettingsService.askRemoveOneCard = !dontAskAgain
            Revise.DeckService.remove_cards([cardId])
        }
    }

    // Remove many cards?
    Revise.ConfirmDialog {
        id: removeCardsDialog

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Удалить выбранные карточки?")
            return qsTr("Удалить все карточки?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Выбранные (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточки будут удалены без возможности восстановления`)
            qsTr("Все карточки будут удалены без возможности восстановления")
        }

        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveManyCards

        onAccepted: {
            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.remove_cards(Revise.CardsModel.ids());
            } else {
                Revise.DeckService.remove_cards(Revise.CardsModel.selectedIds());
            }

            Revise.SettingsService.askRemoveManyCards = !dontAskAgain
        }
    }

    // Relearn one card?
    Revise.ConfirmDialog {
        id: relearnCardDialog

        property int cardId

        titleText: qsTr("Учить карточку заново?")
        messageText: qsTr("Прогресс обучения будет сброшен для этой карточки")
        confirmButtonText: qsTr("Учить заново")
        suppressed: !Revise.SettingsService.askRelearnOneCard

        onAccepted: {
            Revise.DeckService.reset_cards([cardId]);
            Revise.SettingsService.askRelearnOneCard = !dontAskAgain
        }
    }

    // Relearn many cards?
    Revise.ConfirmDialog {
        id: relearnCardsDialog

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Учить выбранные карточки заново?")
            return qsTr("Учить все карточки заново?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет сброшен прогресс обучения`)
            qsTr("Для всех карточек будет сброшен прогресс обучения")
        }

        confirmButtonText: qsTr("Учить заново")
        suppressed: !Revise.SettingsService.askRelearnManyCards

        onAccepted: {
            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.reset_cards(Revise.CardsModel.ids());
            } else {
                Revise.DeckService.reset_cards(Revise.CardsModel.selectedIds());
            }

            Revise.SettingsService.askRelearnManyCards = !dontAskAgain
        }
    }

    // Remove deck?
    Revise.ConfirmDialog {
        id: removeDeckDialog

        property int deckId

        titleText: qsTr("Удалить колоду?")
        messageText: qsTr("Колода будет удалена без возможности восстановления")
        confirmButtonText: qsTr("Удалить")
        suppressed: !Revise.SettingsService.askRemoveDeck

        onAccepted: {
            Revise.DeckService.remove_deck(deckId)
            Revise.Router.navigate("home", {}, Revise.page.Page)
            Revise.SettingsService.askRemoveDeck = !dontAskAgain
        }
    }

    // Set difficulty for many cards?
    Revise.ConfirmDialog {
        id: setDifficultyForCardsDialog

        property real difficulty

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Задать сложность выбранным карточкам?")
            return qsTr("Задать сложность всем карточкам?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет установлена сложность ${difficulty.toFixed(1)}`)
            qsTr(`Для всех карточек будет установлена сложность ${difficulty.toFixed(1)}`)
        }

        confirmButtonText: qsTr("Задать")
        suppressed: !Revise.SettingsService.askSetDifficultyForManyCards

        onAccepted: {
            let draft = new Revise.updateCardDraft();
            draft.difficulty = difficulty;

            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
            } else {
                Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
            }

            Revise.SettingsService.askSetDifficultyForManyCards = !dontAskAgain
        }
    }

    // Set state for many cards?
    Revise.ConfirmDialog {
        id: setStateForCardsDialog

        property string state

        titleText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr("Задать статус выбранным карточкам?")
            return qsTr("Задать статус всем карточкам?")
        }

        messageText: {
            if (Revise.CardsModel.selectedCount > 0)
                return qsTr(`Для выбранных (${Revise.CardsModel.selectedCount} из ${Revise.CardsModel.cardsCount}) карточек будет установлен статус «${state}»`)
            qsTr(`Для всех карточек будет установлен статус «${state}»`)
        }

        confirmButtonText: qsTr("Задать")
        suppressed: !Revise.SettingsService.askSetStateForManyCards

        onAccepted: {
            let draft = new Revise.updateCardDraft();

            if (state === qsTr("Новая"))
                draft.state = Revise.card.State.New
            else if (state === qsTr("Повторяемая"))
                draft.state = Revise.card.State.Review
            else if (state === qsTr("Ошибочная"))
                draft.state = Revise.card.State.Incorrect

            if (Revise.CardsModel.selectedCount === 0) {
                Revise.DeckService.update_cards(Revise.CardsModel.ids(), draft);
            } else {
                Revise.DeckService.update_cards(Revise.CardsModel.selectedIds(), draft);
            }

            Revise.SettingsService.askSetStateForManyCards = !dontAskAgain
        }
    }
}
