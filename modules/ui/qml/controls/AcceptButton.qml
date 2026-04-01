// A accept button with text

import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Revise as Revise

Revise.Button {
    id: root

    background.color: hovered ? Qt.lighter(
                                    Revise.Theme.acceptButtonBackground) : Revise.Theme.acceptButtonBackground
    background.border.color: Qt.lighter(Revise.Theme.acceptButtonBackground)
    background.opacity: 0.9
    color: Revise.Theme.acceptButtonTextColor
}
