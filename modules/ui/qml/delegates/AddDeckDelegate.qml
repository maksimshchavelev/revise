// AddDeckDelegate.qml

import QtQuick
import Revise as Revise

Loader {
    id: root

    property bool loading: false
    property string header: qsTr("Добавить колоду")
    property string description: qsTr("Вы можете создать новую колоду или импортировать существующую из Anki или Revise")

    signal importClicked
    signal createClicked

    sourceComponent: Revise.SettingsService.useMobileUi ? mobileDelegate : desktopDelegate

    Component {
        id: desktopDelegate
        Revise.DesktopAddDeckDelegate {}
    }

    Component {
        id: mobileDelegate
        Revise.MobileAddDeckDelegate {}
    }

    Binding {
        target: root.item
        property: "loading"
        value: root.loading
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "header"
        value: root.header
        when: root.item !== null
    }

    Binding {
        target: root.item
        property: "description"
        value: root.description
        when: root.item !== null
    }

    Connections {
        target: root.item

        function onImportClicked() {
            root.importClicked()
        }

        function onCreateClicked() {
            root.createClicked()
        }
    }
}
