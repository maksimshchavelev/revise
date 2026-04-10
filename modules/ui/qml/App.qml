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

    title: pageLoader.currentWindowTitle

    Revise.Loader {
        id: pageLoader
        anchors.fill: parent

        onLoadingStarted: loadingScreen.startLoading()
        onLoadingFinished: loadingScreen.endLoading()
    }

    Revise.LoadingScreen {
        id: loadingScreen
        anchors.fill: parent
    }

    Item {
        id: windowLayer
    }

    Connections {
        target: router

        function onPageChanged() {
            pageLoader.requestPage(router.currentPageComponent, router.currentPage.params)
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

    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_Escape) {
            event.accepted = true
            router.back()
        }
    }
}
