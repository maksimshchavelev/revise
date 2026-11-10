import QtQuick
import QtWebView
import QtCore
import Revise as Revise

Item {
    id: root

    property string editor: StandardPaths.writableLocation(
                                StandardPaths.AppDataLocation) + "/web/card_editor/index.html"

    WebView {
        id: web

        anchors.fill: parent
        visible: !loading
        settings.allowFileAccess: true

        Component.onCompleted: {
            web.url = Qt.resolvedUrl(root.editor)
        }
    }

    layer.effect: Revise.Shadow {}

    function setHtml(html) {
        html = html
                .replace(/\\/g, '\\\\')
                .replace(/'/g, "\\'")
                .replace(/"/g, '\\"')
                .replace(/\n/g, '\\n')
                .replace(/\r/g, '\\r')
        web.runJavaScript(`editor.setHtml('${html}')`)
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
