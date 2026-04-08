// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString> // for QString

namespace utils {

/**
 * @brief Class with utilities for working with html
 */
class Html {
  public:
    /**
     * @brief Replace image sources in HTML according to the provided mapping.
     *
     * The method scans the HTML string for `<img>` tags and replaces the value
     * of the `src` attribute if it exactly matches a key in `mapped_images`.
     *
     * Only the `src` attribute is modified. All other attributes and tags
     * remain unchanged.
     *
     * This method is commonly used when:
     * - importing decks (relative image names → absolute paths),
     * - exporting decks (absolute paths → relative image names),
     * - normalizing image references inside card HTML.
     *
     * @param html Input HTML string.
     * @param mapped_images Mapping where:
     *        - key   — original image source (e.g. "image.png"),
     *        - value — replacement source (e.g. "/abs/path/image.png").
     *
     * @return Modified HTML string with replaced image sources.
     *
     * @note
     * - Matching is **exact** (string equality).
     * - Only `<img src="...">` and `<img src='...'>` are supported.
     * - The method does NOT validate HTML correctness.
     *
     * @par Example: Importing images
     * @code
     * QString html = R"(
     *     <p>Hello</p>
     *     <img src="cat.png">
     *     <img src="dog.png">
     * )";
     *
     * QMap<QString, QString> images{
     *     {"cat.png", "/data/media/deck_1/cat.png"},
     *     {"dog.png", "/data/media/deck_1/dog.png"}
     * };
     *
     * QString result = Html::replace_images_src(html, images);
     * @endcode
     *
     * Result:
     * @code
     * <img src="/data/media/deck_1/cat.png">
     * <img src="/data/media/deck_1/dog.png">
     * @endcode
     *
     * @par Example: Exporting images
     * @code
     * QMap<QString, QString> images{
     *     {"/data/media/deck_1/cat.png", "cat.png"}
     * };
     *
     * html = Html::replace_images_src(html, images);
     * @endcode
     */
    static QString replace_images_src(const QString& html, const QMap<QString, QString>& mapped_images);

    /**
     * @brief Centers HTML text vertically by wrapping it in a special tag
     * @param html HTML (not empty) that needs to be wrapped
     * @return Wrapped HTML
     */
    static QString center_vertical(const QString& html);

    /**
     * @brief Centers HTML text horizontally by wrapping it in a special tag
     * @param html HTML (not empty) that needs to be wrapped
     * @return Wrapped HTML
     */
    static QString center_horizontal(const QString& html);

    /**
     * @brief Counts the number of lines in HTML, taking line break tags into account
     * @param html HTML
     * @return Lines count
     */
    static std::size_t new_lines(const QString& html);

    /**
     * @brief Removes all HTML tags, returning plain text
     * @param html Source HTML
     * @return Processed plain text
     */
    static QString plain_text(QString html);
};

} // namespace utils
