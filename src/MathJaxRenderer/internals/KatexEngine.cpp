// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <MathJaxRenderer/internals/KatexEngine.hpp> // for KatexEngine
#include <QFile>                                     // for QFile

namespace revise::internals {

// Public method
KatexEngine::KatexEngine() {
    QFile f(":/res/math/katex.js");
    if (!f.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(QString("Failed to open %1").arg(f.fileName()).toStdString());
    }

    m_engine.evaluate(QString::fromUtf8(f.readAll()));
}

// Public method
QString KatexEngine::render_to_svg(const QString& latex,
                                   bool           display_mode,
                                   const QString& color) {
    QJSValue katex = m_engine.globalObject().property("katex");

    QJSValue options = m_engine.newObject();
    options.setProperty("displayMode", display_mode);
    options.setProperty("output", "svg");
    options.setProperty("throwOnError", false);

    QJSValue macros = m_engine.newObject();
    macros.setProperty("\\RR", "\\mathbb{R}");
    macros.setProperty("\\NN", "\\mathbb{N}");
    options.setProperty("macros", macros);

    QJSValue katexObj = m_engine.globalObject().property("katex");
    QJSValue renderFn = katexObj.property("renderToString");

    QJSValue result = renderFn.call(
        QJSValueList{ latex, options }
    );

    if (result.isError()) {
        return QString(); // fallback handled
    }

    QString svg = result.toString();

    svg.replace(
        "<svg ",
        QString("<svg fill=\"%1\" ").arg(color)
    );

    return svg;
}

} // namespace revise
