import QtQuick

Item {
    id: root

    property var pendingPageComponent: null
    property var pendingPageParams: null
    property var currentWindowTitle: qsTr("Revise")

    signal loadingStarted
    signal loadingFinished

    Item {
        id: currentPageLayer
        anchors.fill: parent

        Loader {
            id: pageLoader
            anchors.fill: parent
            active: true
            visible: true
            asynchronous: false
        }
    }

    Loader {
        id: preloader
        asynchronous: true
        active: false
        visible: false
    }

    function requestPage(comp, params) {
        pendingPageComponent = comp
        pendingPageParams = params
        preloader.sourceComponent = comp
        preloader.active = true
    }

    Connections {
        target: preloader

        function onStatusChanged() {
            if (preloader.status === Loader.Loading) {
                root.loadingStarted()
            }

            if (preloader.status === Loader.Ready) {
                pageLoader.sourceComponent = pendingPageComponent
                preloader.active = false
                preloader.sourceComponent = undefined
                root.loadingFinished()
            }
        }
    }

    Connections {
        target: pageLoader

        function onLoaded() {
            var newPage = pageLoader.item
            if (!newPage)
                return

            if (newPage.hasOwnProperty("pageParams")) {
                newPage.pageParams = pendingPageParams
            } else {
                console.error("Page hasn't 'pageParams' property")
            }

            if (newPage.hasOwnProperty("openedAsWindow")) {
                newPage.openedAsWindow = false
            } else {
                console.error("Page hasn't 'openedAsWindow' property")
            }

            if (newPage.hasOwnProperty("windowTitle")) {
                root.currentWindowTitle = newPage.windowTitle
            } else {
                console.error("Page hasn't 'windowTitle' property")
            }
        }
    }
}
