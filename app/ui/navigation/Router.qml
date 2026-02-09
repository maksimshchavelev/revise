pragma Singleton

import QtQuick

QtObject {
    property var pageStack: []
    property int currentPage: 0

    function back() {
        if (pageStack.length > 1) {
            pageStack.pop()
            currentPage = pageStack[pageStack.length - 1]
        }
    }

    function goTo(page) {
        pageStack.push(page)
        currentPage = page
    }
}
