import QtQuick

Item {
    id: root

    property var pendingPageComponent: null
    property var pendingPageParams: null
    property var currentWindowTitle: qsTr("Revise")
    property bool frontIsA: true

    signal loadingStarted
    signal loadingFinished

    Loader {
        id: loaderA
        visible: true
        anchors.fill: parent
        asynchronous: true
        onLoaded: handleLoaded()
    }

    Loader {
        id: loaderB
        visible: false
        anchors.fill: parent
        asynchronous: true
        onLoaded: handleLoaded()
    }

    function currentLoader() {
        return frontIsA ? loaderA : loaderB
    }

    function standbyLoader() {
        return frontIsA ? loaderB : loaderA
    }

    function requestPage(comp, params) {
        pendingPageComponent = comp
        pendingPageParams = params
        loadingStarted()

        const back = standbyLoader()
        back.sourceComponent = pendingPageComponent
    }

    function configurePage(page) {
        if ("pageParams" in page)
            page.pageParams = pendingPageParams

        if ("openedAsWindow" in page)
            page.openedAsWindow = false

        if ("windowTitle" in page)
            currentWindowTitle = page.windowTitle
    }

    function handleLoaded() {
        const front = currentLoader()
        const back = standbyLoader()

        front.visible = false
        back.visible = true
        front.sourceComponent = null
        frontIsA = !frontIsA

        configurePage(back.item)
        loadingFinished()
    }
}
