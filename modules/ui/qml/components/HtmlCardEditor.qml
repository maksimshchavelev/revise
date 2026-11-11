// HtmlCardEditor.qml

import QtQuick
import QtWebView
import QtCore
import Revise as Revise

/*!
    \qmltype HtmlCardEditor
    \brief An HTML editor for flashcard content based on QtWebView.
    \inqmlmodule Revise
    \inherits Loader
    \since 1.0

    HtmlCardEditor renders an HTML editing interface inside a WebView.
    It loads a base document from the application data directory and provides
    JavaScript bridging to set, get, and clear the HTML content of the editor.

    The component hides itself while the WebView is loading to prevent
    rendering flicker or showing an empty white page.

    \section1 Example Usage

    \qml
    Revise.HtmlCardEditor {
        id: editor
        anchors.fill: parent

        Component.onCompleted: editor.setHtml("<p>Hello world</p>")
    }
    \endqml
*/
Loader {
    id: root

    /*!
        \qmlproperty string HtmlCardEditor::editor
        \brief The absolute path to the local HTML document used as the editor base.
    */
    property string editor: StandardPaths.writableLocation(
        StandardPaths.AppDataLocation) + "/web/card_editor/index.html"

    /*!
        \qmlproperty bool HtmlCardEditor::ready
        \brief Whether the base document has finished loading.

        This property is set to \c true from the load handler when the base
        document reports \c {WebView.LoadSucceededStatus}, and is \c false
        before that.
    */
    property bool ready: false

    /*!
        \qmlproperty string HtmlCardEditor::pendingHtml
        \brief The HTML that arrived before the base document was ready.

        When \l setHtml is called while the base document is not ready,
        the value is stored here and applied once the load completes.
    */
    property string pendingHtml: ""

    /*!
        \qmlproperty bool HtmlCardEditor::loading
        \brief Whether the underlying WebView is currently loading.
        \readonly
    */
    readonly property bool loading: root.item ? root.item.loading : true

    visible: !loading

    onItemChanged: {
        if (!item) {
            root.ready = false
        }
    }

    sourceComponent: WebView {
        id: web

        anchors.fill: parent
        settings.allowFileAccess: true

        Component.onCompleted: {
            web.url = Qt.resolvedUrl(root.editor)
        }

        onLoadingChanged: function(loadRequest) {
            if (loadRequest.status === WebView.LoadSucceededStatus) {
                root.ready = true

                if (root.pendingHtml !== "") {
                    web.show(root.pendingHtml)
                    root.pendingHtml = ""
                }
            } else if (loadRequest.status === WebView.LoadFailedStatus) {
                root.ready = false
            }
        }

        /*!
            \qmlmethod void HtmlCardEditor::show(string htmlText)
            \brief Renders the given HTML in the editor.
        */
        function show(htmlText) {
            let escaped = htmlText
                .replace(/\\/g, '\\\\')
                .replace(/'/g, "\\'")
                .replace(/"/g, '\\"')
                .replace(/\n/g, '\\n')
                .replace(/\r/g, '\\r')

            web.runJavaScript(`editor.setHtml('${escaped}')`)
        }
    }

    layer.effect: Revise.Shadow {}

    /*!
        \qmlmethod void HtmlCardEditor::setHtml(string html)
        \brief Sets the HTML content in the editor.
    */
    function setHtml(html) {
        if (root.ready && root.item) {
            root.item.show(html)
        } else {
            root.pendingHtml = html
        }
    }

    /*!
        \qmlmethod void HtmlCardEditor::getHtml(var callback)
        \brief Retrieves the current HTML content from the editor.
    */
    function getHtml(callback) {
        if (root.item && root.ready) {
            root.item.runJavaScript(`editor.getHtml()`, function(result) {
                callback(result);
            });
        }
    }

    /*!
        \qmlmethod void HtmlCardEditor::clear()
        \brief Clears the editor's content.
    */
    function clear() {
        if (root.item && root.ready) {
            root.item.runJavaScript(`editor.clear()`)
        }
        root.pendingHtml = ""
    }
}
