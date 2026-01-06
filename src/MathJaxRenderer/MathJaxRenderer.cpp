// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <MathJaxRenderer/MathJaxRenderer.hpp> // for MathJaxRenderer
#include <QSvgRenderer>                        // for QSvgRenderer
#include <QPainter>                            // for QPainter
#include <QGuiApplication>                     // for QGuiApplication
#include <QScreen>                             // for QScreen
#include <QUrl>                                // for QUrl
#include <QFile>                               // for QFile
#include <QTextDocument>                       // for QTextDocument

#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QThread>

#include <QImage>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>

// Qt JS + text/SVG
#include <QJSEngine>
#include <QJSValue>
#include <QTextDocument>
#include <QTextOption>
#include <QRegExp>

namespace revise {

// Public method
MathJaxRenderer::MathJaxRenderer() : QQuickImageProvider(QQuickImageProvider::Image) {}

// Public method
QImage MathJaxRenderer::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    // decode percent-encoded id -> latex
    QString latex = QUrl::fromPercentEncoding(id.toUtf8());
    latex = latex.trimmed();
    latex.remove(QRegExp("[\\x00-\\x1F]"));

    // load katex JS and katex CSS from resources
    QString katex_js;
    QString katex_css;
    {
        QFile fjs(":/res/math/katex.min.js");
        if (!fjs.open(QIODevice::ReadOnly)) {
            qWarning() << "MathJaxRenderer: cannot open katex.min.js";
            return QImage();
        }
        katex_js = QString::fromUtf8(fjs.readAll());
        fjs.close();

        QFile fcss(":/res/math/katex.min.css");
        if (fcss.open(QIODevice::ReadOnly)) {
            katex_css = QString::fromUtf8(fcss.readAll());
            fcss.close();
        } // if css missing we continue; rendering still possible but may look worse
    }

    // prepare JS engine and execute katex loader
    QJSEngine js_engine;
    js_engine.evaluate(katex_js); // katex becomes available in global object

    // prepare options object
    QJSValue options = js_engine.newObject();
    options.setProperty("displayMode", true);
    options.setProperty("throwOnError", false);

    // call katex.renderToString(latex, options)
    QJSValue katex_obj = js_engine.globalObject().property("katex");
    if (!katex_obj.isObject()) {
        qWarning() << "MathJaxRenderer: katex not found in JS engine";
        return QImage();
    }
    QJSValue render_fn = katex_obj.property("renderToString");
    if (!render_fn.isCallable()) {
        qWarning() << "MathJaxRenderer: katex.renderToString not callable";
        return QImage();
    }

    // escape latex for passing as JS string (we'll pass as QJSValue(QString))
    QJSValue result = render_fn.call(QJSValueList{ QJSValue(latex), options });
    if (result.isError()) {
        qWarning() << "MathJaxRenderer: katex error:" << result.toString();
        // fallback: render plain text image
        QString fallback = latex;
        QTextDocument doc_fb;
        doc_fb.setHtml(QString("<div style='color:#d8d8d8;'>%1</div>").arg(fallback.toHtmlEscaped()));
        QSizeF fb_size = doc_fb.size();
        qreal dpr = QGuiApplication::primaryScreen() ? QGuiApplication::primaryScreen()->devicePixelRatio() : 1.0;
        QSize img_size = (fb_size * dpr).toSize();
        if (img_size.width() <= 0) img_size.setWidth(1);
        if (img_size.height() <= 0) img_size.setHeight(1);
        QImage image(img_size, QImage::Format_ARGB32_Premultiplied);
        image.setDevicePixelRatio(dpr);
        image.fill(Qt::transparent);
        QPainter p(&image);
        doc_fb.drawContents(&p);
        if (size) *size = fb_size.toSize();
        return image;
    }

    // KaTeX returned HTML string (with spans). Build full HTML for QTextDocument.
    QString katex_html = result.toString();

    QString full_html = QStringLiteral(
        "<!doctype html><html><head><meta charset='utf-8'><style>body{margin:0;padding:0;background:transparent;}</style>"
        );

    if (!katex_css.isEmpty()) {
        full_html += QString("<style>%1</style>").arg(katex_css);
    }

    full_html += QStringLiteral("</head><body>") + katex_html + QStringLiteral("</body></html>");

    // Create QTextDocument and render
    QTextDocument doc;
    doc.setDefaultTextOption(QTextOption(Qt::AlignLeft));
    doc.setHtml(full_html);

    // determine logical size
    QSizeF logical_size;
    if (requestedSize.isValid()) {
        logical_size = QSizeF(requestedSize);
        doc.setTextWidth(logical_size.width());
    } else {
        doc.setTextWidth(-1);
        logical_size = doc.size();
        if (logical_size.width() <= 0) logical_size.setWidth(100);
        if (logical_size.height() <= 0) logical_size.setHeight(20);
    }

    // device pixel ratio and image creation
    qreal dpr = QGuiApplication::primaryScreen() ? QGuiApplication::primaryScreen()->devicePixelRatio() : 1.0;
    QSize image_size = (logical_size * dpr).toSize();
    if (image_size.width() <= 0) image_size.setWidth(1);
    if (image_size.height() <= 0) image_size.setHeight(1);

    QImage image(image_size, QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(dpr);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRectF target_rect(QPointF(0,0), logical_size);
    doc.drawContents(&painter, target_rect);
    painter.end();

    if (size) *size = logical_size.toSize();
    return image;
}

} // namespace revise
