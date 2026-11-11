// DeckDelegate.qml

import QtQuick
import Revise as Revise

Loader {
    id: root

    property int newCardsProperty
    property int reviewCardsProperty
    property int incorrectCardsProperty
    property int deckIdProperty
    property int timeLimitProperty
    property string deckNameProperty
    property string deckDescriptionProperty
    property bool repeatableTodayProperty

    signal studyClicked
    signal editClicked
    signal removeClicked
    signal exportClicked

    sourceComponent: Revise.SettingsService.useMobileUi ? mobileDelegate : desktopDelegate

    Component {
        id: desktopDelegate
        Revise.DesktopDeckDelegate {}
    }

    Component {
        id: mobileDelegate
        Revise.MobileDeckDelegate {}
    }

    Binding {
        target: root.item
        property: "newCardsProperty"
        value: root.newCardsProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "reviewCardsProperty"
        value: root.reviewCardsProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "incorrectCardsProperty"
        value: root.incorrectCardsProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "deckIdProperty"
        value: root.deckIdProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "timeLimitProperty"
        value: root.timeLimitProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "deckNameProperty"
        value: root.deckNameProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "deckDescriptionProperty"
        value: root.deckDescriptionProperty
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "repeatableTodayProperty"
        value: root.repeatableTodayProperty
        when: root.item !== null
    }

    Connections {
        target: root.item

        function onStudyClicked() {
            root.studyClicked()
        }

        function onEditClicked() {
            root.editClicked()
        }

        function onRemoveClicked() {
            root.removeClicked()
        }

        function onExportClicked() {
            root.exportClicked()
        }
    }
}
