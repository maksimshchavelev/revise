pragma Singleton
import QtQuick

QtObject {
    // App Background
    readonly property color backgroundColor: Qt.color("#0a101e")

    // Card decks, settings panels, dialog boxes, sidebar. This is the primary color of the elements displayed on top of the background
    readonly property color surfaceColor: Qt.color("#111828")

    // Search areas, input fields, secondary panels.
    readonly property color surfaceContainerColor: Qt.color("#1a284a")

    // Main buttons, active switches, highlighted elements, and selection indicators
    readonly property color primaryColor: Qt.color("#464edb")

    // Main text that is easy to read against a background
    readonly property color foregroundColor: Qt.color("#c4c6dd")

    // Secondary text for descriptions and explanations that isn't particularly eye-catching.
    readonly property color secondaryForegroundColor: Qt.color("#828494")

    // Low value of progressbar color
    readonly property color progressbarLowColor: "#f05f32"

    // Medium value of progressbar color
    readonly property color progressbarMediumColor: "#f0dd32"

    // High value of progressbar color
    readonly property color progressbarHighColor: "#81f032"

    // Color or updated streak
    readonly property color streakUpdatedTextColor: "#e09955"

    readonly property color popupBorder: "#5e5e5e"

    // Font size
    readonly property int fontSizeSmall: 13
    readonly property int fontSizeDefault: 16
    readonly property int fontSizeMedium: 20
    readonly property int fontSizeBig: 28
    readonly property int fontSizeGiant: 40

    // Other colors
    readonly property color green: "#6DD23E"
    readonly property color blue: "#3C5CFA"
    readonly property color red: "#E84747"
    readonly property color grey: "#929292"

    // Info toast message background
    readonly property color toastInfo: "#a5a1b0"

    // Success toast message background
    readonly property color toastSuccess: "#4eb374"

    // Error toast message background
    readonly property color toastError: "#d4613e"
}
