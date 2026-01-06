// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QQuickImageProvider>       // for QQuickImageProvider
#include "internals/KatexEngine.hpp" // for KatexEngine

namespace revise {

/**
 * @brief MathJax render class
 * @details The logic is as follows:
 * 1. All MathJax formulas are selected on the QML side.
 * 2. The formulas are converted into URIs for the renderer.
 * 3. The renderer renders the formula from the URI into an image and returns it to QML.
 * 4. QML renders the image instead of the formula.
 */
class MathJaxRenderer : public QQuickImageProvider {
public:
    /**
     * @brief MathJaxRenderer constructor
     * @throws `std::runtime_error` if failed to open `katex.min.js`
     */
    MathJaxRenderer();

    /**
     * @brief Request image
     * @see Qt docs: https://doc.qt.io/qt-6/qquickimageprovider.html#requestImage
     */
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
    internals::KatexEngine m_katex_engine;
};

} // namespace revise
