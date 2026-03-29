pragma Singleton
import QtQuick

QtObject {
    readonly property color tintColor: "#060606"
    readonly property color textColor: "#D8D8D8"
    readonly property color textColorDark: "#929292"

    readonly property color progressbarLowColor: "#f05f32"
    readonly property color progressbarMediumColor: "#f0dd32"
    readonly property color progressbarHighColor: "#81f032"
    readonly property color progressbarBackgroundColor: "#808080"

    readonly property color toolbarColor: "black"

    readonly property color streakTextColor: "#9A9A9A"
    readonly property color streakUpdatedTextColor: "#E25418"

    readonly property color deckTextColor: "#AAAAAA"
    readonly property color deckNewCardsTextColor: "#6DD23E"
    readonly property color deckConsolidateCardsTextColor: "#3C5CFA"
    readonly property color deckIncorrectCardsTextColor: "#E84747"

    readonly property color trainingBarWrong: "#E84747"
    readonly property color trainingBarHard: "#D8D8D8"
    readonly property color trainingBarNormal: "#617BFD"
    readonly property color trainingBarEasy: "#6DD23E"

    readonly property color checkboxBackground: "#A9A9A9"

    readonly property color textInputDefaultBorder: "#4d4d4d"
    readonly property color textInputValidBorder: "#6DD23E"
    readonly property color textInputInvalidBorder: "#E84747"

    readonly property color popupBorder: "#5e5e5e"

    // Font size
    readonly property int textSizeDefault: 10
    readonly property int textSizeMedium: 13
    readonly property int textSizeBig: 18

    readonly property color debugBounds: "#e30000"

    // Background
    readonly property color backgroundGradient1: "#2a285c"
    readonly property color backgroundGradient2: "#5c2831"
    readonly property color background: "#121315"

    // Decks
    readonly property color deckTint: "#4E5156"

    // Borders
    readonly property color buttonBorder: "#4E5156"

    // Other colors
    readonly property color green: "#6DD23E"
    readonly property color blue: "#3C5CFA"
    readonly property color red: "#E84747"
    readonly property color grey: "#929292"
}
