// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QJSEngine> // for QJSEngine

namespace revise::internals {

/**
 * @brief A helper class for KaTeX rendering, which is similar to MathJax.
 */
class KatexEngine {
public:
    /**
     * @brief KatexEngine constructor
     * @throws `std::runtime_error` if failed to open katex.min.js from resources
     */
    KatexEngine();

    QString render_to_svg(const QString& latex,
                          bool           display_mode,
                          const QString& color);

private:
    QJSEngine m_engine;
};

} // namespace revise
