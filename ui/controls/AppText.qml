import QtQuick
import "qrc:/ui/theme"

Text {
    color: Theme.textColor
    font.pixelSize: Theme.textSizeDefault
    font.family: Fonts.regular.name
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
