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

    Loader {
        id: pageLoader
        anchors.fill: parent
        sourceComponent: router.currentPageComponent
        asynchronous: true

        property bool loading: status === Loader.Loading
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
        id: toastServiceConnection
        target: toastService

        function onRequested(msg) {
            toast.header = msg.header
            toast.text = msg.message

            let totalLength = toast.header.length + toast.text.length

            if (msg.type === Revise.toastRequest.INFO) {
                toast.duration = Math.max(2500, totalLength * 1000 / 20)
                toast.color = Revise.Theme.toastInfo
                toast.icon = "qrc:/res/img/info.svg"
            }

            if (msg.type === Revise.toastRequest.SUCCESS) {
                toast.duration = Math.max(2500, totalLength * 1000 / 20)
                toast.color = Revise.Theme.toastSuccess
                toast.icon = "qrc:/res/img/success.svg"
            }

            if (msg.type === Revise.toastRequest.ERROR) {
                toast.duration = Math.max(4000, totalLength * 1000 / 20)
                toast.color = Revise.Theme.toastError
                toast.icon = "qrc:/res/img/error.svg"
            }

            toast.open()
        }
    }

    Revise.Toast {
        id: toast

        x: appWindow.width - toast.width - 5
        y: 10

        onClosed: toastService.closed()
    }
}
