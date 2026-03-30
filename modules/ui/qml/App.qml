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

        // onLoaded: {
        //     if (item && router.currentPage) {
        //         if (item.hasOwnProperty("pageParams")) {
        //             item.pageParams = router.currentPage.params
        //         } else {
        //             console.warn(`Page ${router.currentPage.name} hasn't 'pageParams' property`)
        //         }
        //     }
        // }
    }

    Item {
        id: windowLayer
    }

    function createWindow(page, params) {
        const component = router.currentPageComponent

        const win = Qt.createQmlObject(`
                                       import QtQuick
                                       import QtQuick.Window

                                       Window {
                                       width: 720
                                       height: 540
                                       visible: false
                                       title: "Revise"
                                       minimumWidth: 320
                                       minimumHeight: 240
                                       }
                                       `, windowLayer)

        const item = component.createObject(win)

        if (!item) {
            console.error("Failed to create page object")
            return
        }

        if (item.hasOwnProperty("pageParams")) {
            item.pageParams = params
        } else {
            console.error("Page hasn't 'pageParams' property")
        }

        if (item.hasOwnProperty("windowTitle")) {
            win.title = item.windowTitle
        } else {
            console.error("Page hasn't 'windowTitle' property")
        }

        item.parent = win.contentItem
        item.anchors.fill = win.contentItem

        win.transientParent = appWindow
        win.visible = true
    }

    Connections {
        target: router

        function onPageChanged() {
            if (router.currentPage.mode === Revise.page.Page) {
                pageLoader.sourceComponent = router.currentPageComponent
            } else if (router.currentPage.mode === Revise.page.Window) {
                createWindow(router.currentPageComponent, router.currentPage.params)
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
        running: deckService.importInProgress || deckService.exportInProgress
                 || pageLoader.loading
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
