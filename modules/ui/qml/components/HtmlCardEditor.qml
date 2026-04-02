import QtQuick
import QtWebView
import QtCore
import Qt5Compat.GraphicalEffects
import Revise as Revise

Item {
    id: root

    property string editor: StandardPaths.writableLocation(
                                StandardPaths.AppDataLocation) + "/web/card_editor/index.html"
    property string pendingHtml

    WebView {
        id: web

        anchors.fill: parent
        visible: !loading

        settings.allowFileAccess: true
        settings.localContentCanAccessFileUrls: true

        onLoadingChanged: function (loadRequest) {
            if (loadRequest.status === WebView.LoadSucceededStatus) {
                let html = root.pendingHtml
                    .replace(/\\/g, '\\\\')
                    .replace(/'/g, "\\'")
                    .replace(/"/g, '\\"')
                    .replace(/\n/g, '\\n')
                    .replace(/\r/g, '\\r')
                web.runJavaScript(`editor.setHtml('${html}')`)
            }
        }

        Component.onCompleted: {
            web.url = Qt.resolvedUrl(root.editor)
            console.log(root.editor)
        }
    }

    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 4
        verticalOffset: 4
        radius: 8
        samples: 32
        color: "#80000000"
    }

    function setHtml(html) {
        root.pendingHtml = html
    }

    function getHtml(callback) {
        if (web.loading) {
            return
        }

        web.runJavaScript(`editor.getHtml()`, function(result) {
            callback(result);
        });
    }

    function clear() {
        web.runJavaScript(`editor.clear()`)
    }
}
