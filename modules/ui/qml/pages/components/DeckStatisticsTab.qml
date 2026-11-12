// DeckStatisticsTab.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype DeckStatisticsTab
    \brief A tab displaying statistical information about a deck.
    \inqmlmodule Revise
    \inherits Revise.Flickable
    \since 1.0

    DeckStatisticsTab provides a scrollable view of various deck statistics
    presented as \l Revise.RadialGauge elements. It is designed to be used
    inside a \l StackLayout within the deck editor.

    The tab visualizes the following metrics:
    \list
    \li Overall study progress (percentage of reviewed cards).
    \li The number of new cards.
    \li The number of review cards.
    \li The number of incorrect cards.
    \endlist

    All data is sourced directly from \l Revise.CardsModel. The component has
    no public properties or signals of its own; it automatically updates as
    the underlying model changes.

    \section1 Example Usage

    \qml
    Revise.DeckStatisticsTab {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    \endqml
*/
Revise.Flickable {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    contentWidth: width
    contentHeight: deckStatistics.implicitHeight

    Revise.PanelColumn {
        id: deckStatistics
        width: parent.width - 16

        // Study progress
        Revise.RadialGauge {
            width: 220
            anchors.horizontalCenter: parent.horizontalCenter
            maximum: Revise.CardsModel.cardsCount
            value: Revise.CardsModel.reviewCardsCount
            valueText: Revise.CardsModel.cardsCount > 0 ? qsTr(`${Math.trunc(value * 100 / maximum)}%`) : "0%"
            text: qsTr("Прогресс обучения")
        }

        Revise.HorizontalDivider {
            width: parent.width
        }

        // New cards
        Revise.RadialGauge {
            width: 220
            anchors.horizontalCenter: parent.horizontalCenter
            backgroundColor: Color.darker(Revise.Theme.green, 3)
            foregroundColor: Revise.Theme.green
            value: Revise.CardsModel.newCardsCount
            maximum: Revise.CardsModel.cardsCount
            text: qsTr("Новых карточек")
        }

        // Review cards
        Revise.RadialGauge {
            width: 220
            anchors.horizontalCenter: parent.horizontalCenter
            backgroundColor: Color.darker(Revise.Theme.blue, 3)
            foregroundColor: Revise.Theme.blue
            value: Revise.CardsModel.reviewCardsCount
            maximum: Revise.CardsModel.cardsCount
            text: qsTr("Повторяемых карточек")
        }

        // Incorrect cards
        Revise.RadialGauge {
            width: 220
            anchors.horizontalCenter: parent.horizontalCenter
            backgroundColor: Color.darker(Revise.Theme.red, 3)
            foregroundColor: Revise.Theme.red
            value: Revise.CardsModel.incorrectCardsCount
            maximum: Revise.CardsModel.cardsCount
            text: qsTr("Ошибочных карточек")
        }
    }
}
