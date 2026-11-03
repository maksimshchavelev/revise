import QtQuick
import QtQuick.Controls
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

    Item {
        id: pageHost
        anchors.fill: parent
    }

    Revise.LoadingScreen {
        id: screen
        anchors.fill: parent
        description: Revise.LoadingScreenService.description
    }

    Connections {
        target: Revise.LoadingScreenService

        function onVisibleChanged() {
            if (Revise.LoadingScreenService.visible)
                screen.startLoading()
            else
                screen.endLoading()
        }
    }

    Connections {
        target: Revise.Router

        function onPageChanged() {
            let newPage = Revise.Router.currentPageItem

            newPage.parent = pageHost
            newPage.anchors.fill = pageHost

            if (newPage.hasOwnProperty("windowTitle")) {
                appWindow.title = newPage.windowTitle
            } else {
                console.warn(`Page ${Revise.Router.currentPage.name} hasn't 'windowTitle' property`)
            }
        }
    }

    Connections {
        id: toastServiceConnection
        target: Revise.ToastService

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

        duration: 4000
        onClosed: Revise.ToastService.closed()
    }
}
