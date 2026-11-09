// CardView.qml

import QtQuick
import QtWebView
import QtCore
import Revise as Revise

/*!
    \qmltype CardView
    \brief Displays an HTML flashcard inside a WebView.
    \inqmlmodule Revise
    \inherits WebView
    \since 1.0

    CardView renders one side of a flashcard. The content is HTML, and it
    is displayed inside a WebView that loads a fixed base document from the
    application data directory. The base document exposes JavaScript
    functions that the component uses to inject the HTML, set the font size,
    and set the text color.

    CardView waits until the base document has finished loading before
    pushing any content. If \l html is assigned before the document is
    ready, the value is stored in \l pendingHtml and applied as soon as the
    document finishes loading. Once the document is ready, changing
    \l html, \l fontSize, or \l textColor immediately re-renders the card.

    \section1 Example Usage

    \qml
    Revise.CardView {
        html: Revise.CardEditSession.front
        fontSize: Revise.Theme.fontSizeMedium
        textColor: Revise.Theme.foregroundColor
    }
    \endqml

    \sa Revise.HtmlCardEditor
*/
WebView {
    id: web

    /*!
        \qmlproperty string CardView::html
        \brief The HTML markup to display.

        The value is passed to the base document through
        \l {CardView::show}{show()} after being escaped and URL-encoded, so
        the markup is rendered as content rather than executed as a script.
        Assigning this property before the base document is ready stores the
        value in \l pendingHtml until the load completes.
    */
    property string html

    /*!
        \qmlproperty int CardView::fontSize
        \brief The font size, in pixels, used to render the card.

        The value is forwarded to the base document when the card is shown
        and re-applied on every \l {CardView::show}{show()} call.
    */
    property int fontSize: (Revise.Theme.fontSizeMedium + Revise.Theme.fontSizeBig) / 2

    /*!
        \qmlproperty color CardView::textColor
        \brief The color of the text in the rendered card.

        The value is forwarded to the base document when the card is shown
        and re-applied on every \l {CardView::show}{show()} call.

        The default value is
        \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}.
    */
    property color textColor: Revise.Theme.foregroundColor

    /*!
        \qmlproperty bool CardView::ready
        \brief Whether the base document has finished loading.

        This property is set to \c true from the load handler when the base
        document reports \c {WebView.LoadSucceededStatus}, and is \c false
        before that. While \c ready is \c false, assignments to \l html are
        deferred instead of being applied immediately.

        The default value is \c false.
    */
    property bool ready: false

    /*!
        \qmlproperty string CardView::pendingHtml
        \brief The HTML that arrived before the base document was ready.

        When \l html is assigned while \l ready is \c false, the value is
        stored here and applied once the base document finishes loading.
        When \l ready is \c true, this property is not used and the HTML is
        rendered immediately.

        The default value is an empty string.
    */
    property string pendingHtml: ""

    Component.onCompleted: {
        const base = Qt.resolvedUrl(StandardPaths.writableLocation(StandardPaths.AppDataLocation) + "/web/card/card.html")
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

    /*!
        \qmlmethod void CardView::show(string htmlText)
        \brief Renders the given HTML in the card.

        The method escapes \a htmlText so that it is treated as content
        rather than markup with embedded scripts, URL-encodes it, and passes
        it to the base document together with the current \l fontSize and
        \l textColor. It is called automatically when \l html changes while
        \l ready is \c true, and from the load handler when the base
        document finishes loading.
    */
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
