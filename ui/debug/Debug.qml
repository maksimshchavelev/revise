// Singleton indicating whether debugging is enabled

pragma Singleton
import QtQuick

QtObject {
    readonly property bool showBounds: uiShowBounds // set via context property
}
