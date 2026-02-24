import QtQuick
import Revise

Text {
    color: Theme.textColor
    font.pixelSize: Theme.textSizeDefault
    font.family: Fonts.regular.name
    antialiasing: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    DebugBounds {}
}
