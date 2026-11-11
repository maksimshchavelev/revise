import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Revise as Revise

ApplicationWindow {
    id: appWindow
    visible: true

    minimumWidth: 760
    minimumHeight: 420

    width: 1075
    height: 720

    Rectangle {
        id: pageHost

        anchors.fill: parent
        color: Revise.Theme.backgroundColor
        clip: true

        property real uiScale: Revise.SettingsService.uiScale

        Item {
            id: contentHost

            width: pageHost.width / pageHost.uiScale
            height: Math.max(
                0,
                (pageHost.height - Revise.AndroidKeyboardInsets.imeHeight)
                / pageHost.uiScale
            )

            transform: Scale {
                xScale: pageHost.uiScale
                yScale: pageHost.uiScale
                origin.x: 0
                origin.y: 0
            }

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }
    }

    Revise.LoadingScreen {
        id: screen
        width: appWindow.width / Revise.SettingsService.uiScale
        height: appWindow.height / Revise.SettingsService.uiScale
        transformOrigin: Item.TopLeft
        scale: Revise.SettingsService.uiScale
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

            newPage.parent = contentHost
            newPage.anchors.fill = contentHost

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

        scale: Revise.SettingsService.uiScale
        transformOrigin: Item.TopRight
        y: 12

        duration: 4000
        onClosed: Revise.ToastService.closed()
    }
}
