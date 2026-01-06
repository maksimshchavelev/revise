// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject> // for QObject

namespace revise {

/**
 * @brief Class with auxiliary HTML utilities
 */
class HtmlHelper : public QObject {
Q_OBJECT

public:
    HtmlHelper(QObject* parent = nullptr);

    /**
    * @brief Replace values of img src attributes according to replacements map.
    *
    * This is a simple regexp-based replacement. It matches patterns like:
    *   src="value", src='value' and src=value
    * For each matched value it tries:
    *   1) exact match in replacements (key == full src value)
    *   2) basename match in replacements (key == QFileInfo(src).fileName())
    *
    * @param html Input HTML/text containing <img src=...> references.
    * @param replacements Map of replacements: key -> what to replace, value -> replacement string.
    *                     Keys and values are plain strings (e.g. "paste-abc.jpg" -> "/abs/path/to/file.jpg").
    * @return New string with replaced src values.
    */
    QString replace_image_src(const QString& html, const QMap<QString, QString>& replacements) const;

    /**
    * @brief Insert or replace width attribute on all <img> tags in HTML.
    *
    * This function:
    *  - removes any existing width/height declarations inside `style="..."` for <img>,
    *  - removes any present width="..." or height="..." attributes,
    *  - inserts width="<max_width_px>" attribute into the <img> tag.
    *
    * Note: QTextDocument / Qt RichText reacts better to width attribute than to CSS style width.
    *
    * @param html Input HTML string (may contain many <img ...> tags).
    * @param max_width_px Desired max width in pixels for images (should be available content width minus paddings).
    * @return New HTML string with updated <img> tags.
    */
    Q_INVOKABLE QString scale_images_to_width(const QString& html, int max_width_px) const;

    /**
    * @brief Insert or replace width attribute on all <img> tags in HTML.
    *
    * This function:
    *  - removes any existing width/height declarations inside `style="..."` for <img>,
    *  - removes any present width="..." or height="..." attributes,
    *  - inserts width="<max_width_px>" attribute into the <img> tag.
    *
    * Note: QTextDocument / Qt RichText reacts better to width attribute than to CSS style width.
    *
    * @param html Input HTML string (may contain many <img ...> tags).
    * @param max_height_px Desired max height in pixels for images (should be available content width minus paddings).
    * @return New HTML string with updated <img> tags.
    */
    Q_INVOKABLE QString scale_images_to_height(const QString& html, int max_height_px) const;

    /**
    * @brief Extract MathJax fragments from an HTML string preserving original delimiters.
    *
    * This finds math expressions wrapped with:
    *  - $$ ... $$   (display)
    *  - \[ ... \]   (display)
    *  - \( ... \)   (inline)
    *  - $ ... $     (inline; heuristic, avoids escaped \$)
    *
    * The returned QStringList contains the *exact* matched substrings from the input,
    * in the order they appear (for example: "\\(x^2\\)", "$y+1$", "$$E=mc^2$$").
    *
    * @param html Input HTML text possibly containing MathJax.
    * @return QStringList of MathJax fragments (original, with delimiters).
    */
    Q_INVOKABLE QStringList extract_mathjax(const QString& html);

    /**
     * @brief Replaces all MathJax formulas in the passed `html` text with images
     * @param html HTML
     * @param text_color Color of the text
     * @param img_height Image height
     * @return HTML with replaced mathjax
     * @note The image source will be set to the web server `latex.codecogs.com`
     */
    Q_INVOKABLE QString replace_mathjax_to_images(const QString& html,
                                                  const QString& text_color,
                                                  int img_height);

    /**
     * @brief Checks if there is at least one MathJax in HTML
     * @param html HTML
     * @return `true` if it exists
     */
    Q_INVOKABLE bool has_mathjax(const QString& html);
};

}
