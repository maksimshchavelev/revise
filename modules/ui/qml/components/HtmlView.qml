import QtQuick
import QtWebView
import QtCore
import Revise as Revise

WebView {
    id: web

    property string html
    property int fontSize: Revise.Theme.fontSizeMedium
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

    function centerHtml(htmlText) {
        return `
            <div style="display: grid; place-items: center; height: 100vh;">
                <p>` + htmlText + `</p>
            </div>`
    }

    function limitTextLength(text, maxLength) {
        const tokens = [];
        let lastIndex = 0;
        let index = 0;

        while (index < text.length) {
            const tagMatch = text.substring(index).match(/^<[^>]*>/);
            const mathMatch = text.substring(index).match(/^\$\$[\s\S]*?\$\$|^\\\[[\s\S]*?\\\]|^\\\([\s\S]*?\\\)/);

            if (tagMatch) {
                if (index > lastIndex) {
                    tokens.push({type:'text', content: text.substring(lastIndex, index)});
                }
                tokens.push({type:'tag', content: tagMatch[0]});
                lastIndex = index + tagMatch[0].length;
                index = lastIndex;
            } else if (mathMatch) {
                if (index > lastIndex) {
                    tokens.push({type:'text', content: text.substring(lastIndex, index)});
                }
                tokens.push({type:'math', content: mathMatch[0]});
                lastIndex = index + mathMatch[0].length;
                index = lastIndex;
            } else {
                index++;
            }
        }

        if (lastIndex < text.length) {
            tokens.push({type:'text', content: text.substring(lastIndex)});
        }

        let result = '';
        let currentLine = '';
        let currentLength = 0;

        for (let token of tokens) {
            if (token.type === 'text') {
                const words = token.content.split(/(\s+)/);

                for (let word of words) {
                    if (/^\s+$/.test(word)) {
                        currentLine += word;
                    } else if (word.length > maxLength) {
                        if (currentLength > 0) {
                            result += currentLine + '<br>';
                            currentLine = '';
                            currentLength = 0;
                        }
                        for (let i = 0; i < word.length; i += maxLength) {
                            const chunk = word.substring(i, i + maxLength);
                            result += chunk + '<br>';
                        }
                    } else {
                        if (currentLength + word.length <= maxLength) {
                            currentLine += word;
                            currentLength += word.length;
                        } else {
                            if (currentLine.trim()) {
                                result += currentLine + '<br>';
                            }
                            currentLine = word;
                            currentLength = word.length;
                        }
                    }
                }
            } else {
                currentLine += token.content;
            }
        }

        if (currentLine.trim()) {
            result += currentLine;
        }

        return result.replace(/(<br>)+$/, '');
    }

    function verticalCenterText(text) {
        return `<div style="display: flex; flex-direction: column; justify-content: center; min-height: 100vh;">
            <div style="text-align: left;">${text}</div>
        </div>`;
    }

    function countLines(text) {
        const normalized = text.replace(/<br\s*\/?>/gi, '\n')
                               .replace(/<\/p>|<\/div>|<\/tr>|<\/li>|<\/h[1-6]>/gi, '\n')
                               .replace(/<[^>]+>/g, '')
                               .replace(/\r\n/g, '\n');
        const lines = normalized.split('\n').filter(line => line.trim() !== '');
        return lines.length || (text.length > 0 ? 1 : 0);
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

