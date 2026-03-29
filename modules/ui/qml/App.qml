import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Dialogs
import QtQuick.Layouts
import Revise as Revise

ApplicationWindow {
    id: appWindow
    visible: true

    minimumWidth: 480
    minimumHeight: 320

    width: 1080
    height: 720

    title: "Revise"

    // background: Revise.Background {}

    Loader {
        id: pageLoader
        anchors.fill: parent
        sourceComponent: router.currentPageComponent
        asynchronous: true

        property bool loading: status === Loader.Loading

        onLoaded: {
            if (item && router.currentPage) {
                if (item.hasOwnProperty("pageParams")) {
                    item.pageParams = router.currentPage.params
                } else {
                    console.warn(`Page ${router.currentPage.name} hasn't 'pageParams' property`)
                }
            }
        }
    }

    Revise.InfoPopup {
        id: scrollableInfoPopup
        backgroundItem: pageLoader
    }

    Rectangle {
        id: loaderScreen
        color: "black"
        visible: pageLoader.status === Loader.Loading
        anchors.fill: parent
    }

    BusyIndicator {
        anchors.centerIn: parent
        width: 128
        height: 128
        running: deckService.importInProgress || deckService.exportInProgress || pageLoader.loading
    }

    Connections {
        id: popupServiceConnection
        target: popupService

        function onRequested(req) {
            if (req.type === PopupType.INFO) {
                infoPopup.open(req.message)
            } else {
                console.warn("Unknown popup type")
            }
        }
    }

    Connections {
        id: popupConnection
        target: scrollableInfoPopup

        function onClosed() {
            let action = new PopupAction("closed")
            let response = new PopupResponse(action)

            popupService.response(response)
        }
    }
}
