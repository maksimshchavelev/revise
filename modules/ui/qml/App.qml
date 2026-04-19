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
    }

    Connections {
        target: loadingScreen

        function onVisibleChanged() {
            if (loadingScreen.visible)
                screen.startLoading()
            else
                screen.endLoading()
        }
    }

    Connections {
        target: router

        function onPageChanged() {
            let newPage = router.currentPageItem

            newPage.parent = pageHost
            newPage.anchors.fill = pageHost

            if (newPage.hasOwnProperty("windowTitle")) {
                appWindow.title = newPage.windowTitle
            } else {
                console.warn(`Page ${router.currentPage.name} hasn't 'windowTitle' property`)
            }
        }
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

    Item {
        focus: true

        Keys.onBackPressed: function (event) {
            event.accepted = true
            router.back()
        }
    }
}
