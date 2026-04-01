import QtQuick
import QtWebView
import QtCore
import Revise as Revise

WebView {
    id: web

    property string html
    property int fontSize: Revise.Theme.fontSizeBig
    property color textColor: Revise.Theme.textColor
    property bool ready: false

    property string pendingHtml: ""

    settings.allowFileAccess: true
    settings.localContentCanAccessFileUrls: true

    Component.onCompleted: {
        const base = Qt.resolvedUrl(StandardPaths.writableLocation(StandardPaths.AppDataLocation) + "/web/card.html")
        web.url = base
    }

    onLoadingChanged: function(loadRequest) {
        if (loadRequest.status === WebView.LoadSucceededStatus) {
            web.ready = true

            web.runJavaScript(`setFontSize(${web.fontSize})`)
            web.runJavaScript(`setTextColor('${web.textColor}')`)
            show(html)
        }
    }

    function show(htmlText) {
        var escaped = htmlText.replace(/'/g, "&#39;")
        escaped = encodeURIComponent(escaped)
        escaped = escaped.replace(/"/g, "&quot;")
        escaped = escaped.replace(/</g, "&lt;")
        escaped = escaped.replace(/>/g, "&gt;")
        escaped = escaped.replace(/&/g, "&amp;")

        web.runJavaScript(`setFontSize(${web.fontSize})`)
        web.runJavaScript(`setTextColor('${web.textColor}')`)
        web.runJavaScript(`renderCard(decodeURIComponent('${escaped}'))`);
    }

    onHtmlChanged: {
        if (web.ready) {
            show(html)
        } else {
            pendingHtml = html
        }
    }
}

