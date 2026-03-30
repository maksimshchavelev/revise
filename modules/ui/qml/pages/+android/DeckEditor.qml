// Edit deck page

import QtQuick
import QtQuick.Layouts
import Revise as Revise

Item {
    id: root

    property var pageParams: null

    Revise.DeckEditorContent {
        anchors.fill: parent
        pageParams: root.pageParams
    }
}
