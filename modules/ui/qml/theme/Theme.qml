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
    readonly property color streakUpdatedTextColor: "#e09955"

    readonly property color deckTextColor: "#AAAAAA"
    readonly property color deckNewCardsTextColor: "#6DD23E"
    readonly property color deckConsolidateCardsTextColor: "#3C5CFA"
    readonly property color deckIncorrectCardsTextColor: "#E84747"

    readonly property color trainingBarWrong: "#E84747"
    readonly property color trainingBarHard: "#D8D8D8"
    readonly property color trainingBarNormal: "#617BFD"
    readonly property color trainingBarEasy: "#6DD23E"

    readonly property color checkboxBackground: "#A9A9A9"

    readonly property color popupBorder: "#5e5e5e"

    // Font size
    readonly property int fontSizeSmall: 10
    readonly property int fontSizeDefault: 13
    readonly property int fontSizeMedium: 16
    readonly property int fontSizeBig: 22

    readonly property color debugBounds: "#e30000"

    // Background
    readonly property color backgroundGradient1: "#2a285c"
    readonly property color backgroundGradient2: "#5c2831"
    readonly property color background: "#121315"
    readonly property color backgroundLight: "#1c1e21"

    // Decks
    readonly property color deckTint: "#4E5156"

    // Cards
    readonly property color cardTint: "#4E5156"

    // Buttons
    readonly property color buttonBorder: "#4E5156"
    readonly property color acceptButtonTextColor: "#51c44b"

    // Other colors
    readonly property color green: "#6DD23E"
    readonly property color blue: "#3C5CFA"
    readonly property color red: "#E84747"
    readonly property color grey: "#929292"
    readonly property color darkGrey: "#1c1c1f"

    // Text input
    readonly property color textFieldBackground: "#1c1c1f"
    readonly property color textFieldBorder: "#47474f"

    // Toast
    readonly property color toastInfo: "#a5a1b0"
    readonly property color toastSuccess: "#4eb374"
    readonly property color toastError: "#d4613e"

    // Scrollbar
    readonly property color scrollbarBackground: "#1c1e21"
    readonly property color scrollbar: "#292c30"

    // Loading icon button
    readonly property color loadingIconButtonBackground: "#1c1e21"

    // Sizes
    readonly property int settingsLabelWidth: 220
}
