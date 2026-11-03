// CardDelegate.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype CardDelegate
    \brief A list item that displays a flashcard with status, difficulty, and due date.
    \inqmlmodule Revise
    \inherits Rectangle
    \since 1.0

    CardDelegate renders a single flashcard inside a card list. It shows
    the front side of the card, its status, the next review date, and the
    current difficulty. A checkbox on the left allows the card to be
    selected, and a kebab menu on the right exposes additional actions
    such as previewing, editing, changing difficulty, and removing the
    card.

    The background color is derived from \l status: green for new cards,
    blue for reviewable cards, and red for cards marked as failed. The
    inner panel lightens when \l selected is \c true, and the whole
    delegate slightly scales up on hover.

    The component is designed to be used as a delegate inside a
    \l ListView. To preserve the \l selected state across delegate
    reuse, bind it to a role in the model rather than leaving it as
    local state.

    \section1 Example Usage

    \qml
    ListView {
        model: cardsModel

        delegate: Revise.CardDelegate {
            cardId: model.id
            front: model.front
            back: model.back
            nextReview: model.nextReview
            difficulty: model.difficulty
            status: model.status
            selected: model.isSelected

            onPreviewClicked: openPreview(cardId)
            onEditClicked: openEditor(cardId)
        }
    }
    \endqml

    \sa Revise.KebabMenu, Revise.CheckBox
*/
Rectangle {
    id: root

    /*!
        \qmlproperty bool CardDelegate::pressed
        \brief Whether the delegate is currently pressed.

        This is a read-only convenience property backed by the internal
        \l TapHandler. It reflects the raw pointer press state and does
        not distinguish between press and click.
    */
    property bool pressed: tapHandler.pressed

    /*!
        \qmlproperty bool CardDelegate::hovered
        \brief Whether the pointer is hovering over the delegate.

        This is a read-only convenience property backed by the internal
        \l HoverHandler. When \c true, the delegate scales up slightly
        to indicate interactivity.
    */
    property bool hovered: hoverHandler.hovered

    /*!
        \qmlproperty string CardDelegate::front
        \brief The text on the front side of the flashcard.

        Displayed in the header row of the delegate. The text is
        rendered with a single line and elided on the right if it does
        not fit, and the full text is available through the attached
        \l Revise.HoverableTooltip.
    */
    property string front

    /*!
        \qmlproperty string CardDelegate::back
        \brief The text on the back side of the flashcard.

        Not rendered by the delegate itself. It is provided so that
        consumers can use the same data binding for both sides of the
        card without keeping the back text in a separate model role.
    */
    property string back

    /*!
        \qmlproperty date CardDelegate::nextReview
        \brief The date when the card is next due for review.

        Displayed in the info section as \c {dd.MM.yyyy}. If the date is
        earlier than or equal to the current time, it is highlighted in
        \l {Revise.Theme::blue}{Revise.Theme.blue} to indicate that the
        card is due.
    */
    property date nextReview

    /*!
        \qmlproperty double CardDelegate::difficulty
        \brief The difficulty of the card.

        Expected to be in the range \c {[0, 5]}. Used to compute the
        color of the value via
        \l {Revise.ColorUtils::colorForRange}{Revise.ColorUtils.colorForRange},
        and to enable or disable the increase/decrease difficulty
        actions in the kebab menu.
    */
    property double difficulty

    /*!
        \qmlproperty int CardDelegate::cardId
        \brief The identifier of the card.
        \default 0

        Passed through to consumers via the action signals so that the
        owning view can resolve the card in the deck service.
    */
    property int cardId: 0

    /*!
        \qmlproperty int CardDelegate::status
        \brief The status of the card.
        \default 0

        The value selects the background color of the delegate and the
        label rendered in the status row:

        \list
        \li \c 0 — new card, rendered in
            \l {Revise.Theme::green}{Revise.Theme.green}.
        \li \c 1 — reviewable card, rendered in
            \l {Revise.Theme::blue}{Revise.Theme.blue}.
        \li \c 2 — failed card, rendered in
            \l {Revise.Theme::red}{Revise.Theme.red}.
        \endlist
    */
    property int status: 0

    /*!
        \qmlproperty bool CardDelegate::selected
        \brief Whether the card is selected.

        This property is an alias to the internal checkbox state, so it
        can be read and written from outside and stays in sync with the
        checkbox in both directions. When bound to a model role, the
        selection state survives delegate reuse during scrolling.
    */
    property alias selected: selectCheckBox.checked

    /*!
        \qmlsignal CardDelegate::previewClicked()
        \brief Emitted when the user selects the preview action from the kebab menu.
    */
        signal previewClicked

    /*!
        \qmlsignal CardDelegate::increaseDifficultyClicked()
        \brief Emitted when the user selects the increase difficulty action.

        The action is available only when \l difficulty is less than
        \c 5.
    */
        signal increaseDifficultyClicked

    /*!
        \qmlsignal CardDelegate::decreaseDifficultyClicked()
        \brief Emitted when the user selects the decrease difficulty action.

        The action is available only when \l difficulty is greater than
        \c 0.
    */
        signal decreaseDifficultyClicked

    /*!
        \qmlsignal CardDelegate::editClicked()
        \brief Emitted when the user selects the edit action from the kebab menu.
    */
        signal editClicked

    /*!
        \qmlsignal CardDelegate::removeClicked()
        \brief Emitted when the user selects the remove action from the kebab menu.
    */
        signal removeClicked

    implicitHeight: 100

    color: root.statusColor()
    radius: 12
    scale: root.hovered ? 1.01 : 1.0

    // Main background panel, overlapping the card's color status panel
    Item {
        anchors.fill: parent
        anchors.leftMargin: 8

        // Background tint
        Rectangle {
            // Note: Indents are needed to avoid a visual glitch where the card's background slightly overlaps the edges.
            width: parent.width + 2
            height: parent.height + 2
            anchors.centerIn: parent
            radius: 12

            color: {
                if (root.selected)
                    return Color.lighter(Revise.Theme.surfaceColor, 1.2)
                return Revise.Theme.surfaceColor
            }

            layer.enabled: true
            layer.effect: Revise.Shadow {}

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: 110
                }
            }
        }

        // Select a card from the list of cards
        Revise.CheckBox {
            id: selectCheckBox
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter

            onChanged: (value) => {
                if (value)
                    Revise.CardsModel.selectCard(root.cardId)
                else
                    Revise.CardsModel.deselectCard(root.cardId)
            }
        }

        // Card Information
        Column {
            id: layout
            width: parent.width - selectCheckBox.width - selectCheckBox.anchors.leftMargin - anchors.leftMargin
            spacing: 8

            anchors.left: selectCheckBox.right
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter

            // The front of the card and the menu
            Row {
                width: parent.width
                spacing: 10

                Revise.Text {
                    text: root.front
                    font.bold: true
                    font.pixelSize: Revise.Theme.fontSizeMedium
                    maximumLineCount: 1
                    elide: Revise.Text.ElideRight
                    width: parent.width - menuButton.width - parent.spacing

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: root.front
                    }
                }

                Revise.IconButton {
                    id: menuButton
                    size: 24
                    icon.source: "qrc:/res/img/menu.svg"
                    icon.color: Revise.Theme.textColorDark
                    background.color: "transparent"
                    onClicked: {
                        kekabMenu.x = menuButton.x - kekabMenu.implicitWidth - 20
                        kekabMenu.y = menuButton.y

                        kekabMenu.clampPosition()
                        kekabMenu.open()
                    }
                }
            }

            // Status, difficulty, and due date of the flashcard
            Column {
                spacing: 6
                width: parent.width

                property int optionsWidth: 140

                Revise.KeyValue {
                    width: parent.width
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Статус:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.horizontalAlignment: Text.AlignLeft
                    value.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: {
                        if (root.status === 0)
                            return qsTr("новая")
                        if (root.status === 1)
                            return qsTr("повторяемая")
                        if (root.status === 2)
                            return qsTr("ошибочная")
                    }

                    value.color: root.statusColor()
                }

                Revise.KeyValue {
                    width: parent.width
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Следующий повтор:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.text: Qt.formatDateTime(root.nextReview, "dd.MM.yyyy")
                    value.horizontalAlignment: Text.AlignLeft
                    value.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.color: {
                        let date = new Date()

                        if (root.nextReview.getTime() <= date.getTime()) {
                            return Revise.Theme.blue
                        }

                        return Revise.Theme.textColorDark
                    }
                }

                Revise.KeyValue {
                    width: parent.width
                    keyWidth: parent.optionsWidth
                    key.text: qsTr("Сложность:")
                    key.font.pixelSize: Revise.Theme.fontSizeSmall

                    value.horizontalAlignment: Text.AlignLeft
                    value.text: root.difficulty.toFixed(1)
                    value.font.pixelSize: Revise.Theme.fontSizeSmall
                    value.color: Revise.ColorUtils.colorForRange(
                        root.difficulty, 0, 5, Revise.Theme.green,
                        Revise.Theme.red)
                }
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
            text: qsTr("Превью")
            onClicked: {
                kekabMenu.close()
                root.previewClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Редактировать")
            onClicked: {
                kekabMenu.close()
                root.editClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Повысить сложность")
            clickable: root.difficulty < 5
            onClicked: {
                kekabMenu.close()
                root.increaseDifficultyClicked()
            }
        }

        Revise.KebabMenuButton {
            text: qsTr("Понизить сложность")
            clickable: root.difficulty > 0
            onClicked: {
                kekabMenu.close()
                root.decreaseDifficultyClicked()
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

    TapHandler {
        id: tapHandler
    }

    HoverHandler {
        id: hoverHandler
    }

    /*!
        \qmlmethod color CardDelegate::statusColor()
        \brief Returns the theme color corresponding to the current \l status.

        The mapping is the same as the one used by \l status: green for
        \c 0, blue for \c 1, and red for any other value.
    */
    function statusColor() {
        switch (root.status) {
            case 0:  return Revise.Theme.green
            case 1:  return Revise.Theme.blue
            default: return Revise.Theme.red
        }
    }
}
