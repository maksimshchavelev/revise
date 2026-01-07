// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <HtmlHelper/HtmlHelper.hpp>       // for HtmlHelper
#include <QRegularExpressionMatchIterator> // for QRegularExpressionMatchIterator
#include <QFileInfo>                       // for QFileInfo
#include <QUrl>                            // for QUrl

namespace revise {

// Public method
HtmlHelper::HtmlHelper(QObject *parent) : QObject(parent) {}

// Public method
QString HtmlHelper::replace_image_src(const QString &html, const QMap<QString, QString> &replacements) const
{
    // regex: src = optional whitespace = optional quote + value (not containing whitespace or > or quotes) + same optional quote
    QRegularExpression re(R"(src\s*=\s*(['"]?)([^'">\s]+)\1)", QRegularExpression::CaseInsensitiveOption);

    QString result;
    int last_pos = 0;

    QRegularExpressionMatchIterator it = re.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int match_start = match.capturedStart(0);
        int match_end = match.capturedEnd(0);
        QString quote = match.captured(1);      // may be empty
        QString src_value = match.captured(2);  // the actual src value

        // choose replacement: exact match first, then basename
        QString replacement_value;
        if (replacements.contains(src_value)) {
            replacement_value = replacements.value(src_value);
        } else {
            QString base = QFileInfo(src_value).fileName();
            if (replacements.contains(base)) {
                replacement_value = replacements.value(base);
            }
        }

        // append everything before this match
        result += html.mid(last_pos, match_start - last_pos);

        if (replacement_value.isEmpty()) {
            // no replacement -> keep original match as-is
            result += html.mid(match_start, match_end - match_start);
        } else {
            // preserve quote style if present, otherwise use double quotes
            QString q = quote.isEmpty() ? QStringLiteral("\"") : quote;
            QString new_fragment = QStringLiteral("src=%1%2%1").arg(q, replacement_value);
            result += new_fragment;
        }

        last_pos = match_end;
    }

    // append tail
    result += html.mid(last_pos);
    return result;
}

// Public method
QString HtmlHelper::scale_images_to_width(const QString &html, int max_width_px) const
{
    if (max_width_px <= 0) {
        return html;
    }

    // matches <img ...> capturing attributes
    QRegularExpression img_re(R"(<img\b([^>]*)>)", QRegularExpression::CaseInsensitiveOption);
    // matches style="..."
    QRegularExpression style_re(R"(style\s*=\s*(['"])(.*?)\1)",
                                QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    // remove width:...; and height:...; inside style
    QRegularExpression wh_in_style_re(R"((?:width|height)\s*:\s*[^;]+;?)",
                                      QRegularExpression::CaseInsensitiveOption);
    // attributes width="..." or width=123  (with or without quotes)
    QRegularExpression width_attr_re(R"(\bwidth\s*=\s*(['"])?\d+\1)",
                                     QRegularExpression::CaseInsensitiveOption);
    QRegularExpression height_attr_re(R"(\bheight\s*=\s*(['"])?\d+\1)",
                                      QRegularExpression::CaseInsensitiveOption);

    QString result;
    int last_pos = 0;

    QRegularExpressionMatchIterator it = img_re.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        int start = m.capturedStart(0);
        int end = m.capturedEnd(0);
        QString attrs = m.captured(1); // attributes part (may be empty or contain leading space)

        // append text before match
        result += html.mid(last_pos, start - last_pos);

        QString new_attrs = attrs;

        // 1) remove width/height attributes if present
        new_attrs.remove(width_attr_re);
        new_attrs.remove(height_attr_re);

        // 2) if style attribute exists, remove width/height declarations inside it
        QRegularExpressionMatch style_m = style_re.match(new_attrs);
        if (style_m.hasMatch()) {
            QString quote = style_m.captured(1);
            QString style_val = style_m.captured(2);
            // remove width/height declarations inside style
            style_val.remove(wh_in_style_re);
            style_val = style_val.trimmed();
            if (!style_val.isEmpty() && !style_val.endsWith(';'))
                style_val += ';';

            // keep trimmed style (without width/height)
            QString replacement = QStringLiteral("style=%1%2%1").arg(quote, style_val);
            // replace whole old style occurrence with cleaned version
            new_attrs.replace(style_m.captured(0), replacement);
        }

        // 3) ensure single leading space (attributes string may or may not start with space)
        new_attrs = new_attrs.trimmed();
        if (!new_attrs.isEmpty())
            new_attrs = " " + new_attrs;

        // 4) append width attribute
        new_attrs += QStringLiteral(" width=\"%1\"").arg(max_width_px);

        // reconstruct tag
        QString new_img = QStringLiteral("<img%1>").arg(new_attrs);
        result += new_img;

        last_pos = end;
    }

    // append remainder
    result += html.mid(last_pos);
    return result;
}

// Public method
QString HtmlHelper::scale_images_to_height(const QString& html, int max_height_px) const
{
    if (max_height_px <= 0) {
        return html;
    }

    // matches <img ...> capturing attributes
    QRegularExpression img_re(R"(<img\b([^>]*)>)", QRegularExpression::CaseInsensitiveOption);
    // matches style="..."
    QRegularExpression style_re(R"(style\s*=\s*(['"])(.*?)\1)",
                                QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    // remove width:...; and height:...; inside style
    QRegularExpression wh_in_style_re(R"((?:width|height)\s*:\s*[^;]+;?)",
                                      QRegularExpression::CaseInsensitiveOption);
    // attributes width="..." or width=123  (with or without quotes)
    QRegularExpression width_attr_re(R"(\bwidth\s*=\s*(['"])?\d+\1)",
                                     QRegularExpression::CaseInsensitiveOption);
    QRegularExpression height_attr_re(R"(\bheight\s*=\s*(['"])?\d+\1)",
                                      QRegularExpression::CaseInsensitiveOption);

    QString result;
    int last_pos = 0;

    QRegularExpressionMatchIterator it = img_re.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        int start = m.capturedStart(0);
        int end = m.capturedEnd(0);
        QString attrs = m.captured(1); // attributes part (may be empty or contain leading space)

        // append text before match
        result += html.mid(last_pos, start - last_pos);

        QString new_attrs = attrs;

        // 1) remove width/height attributes if present
        new_attrs.remove(width_attr_re);
        new_attrs.remove(height_attr_re);

        // 2) if style attribute exists, remove width/height declarations inside it
        QRegularExpressionMatch style_m = style_re.match(new_attrs);
        if (style_m.hasMatch()) {
            QString quote = style_m.captured(1);
            QString style_val = style_m.captured(2);
            // remove width/height declarations inside style
            style_val.remove(wh_in_style_re);
            style_val = style_val.trimmed();
            if (!style_val.isEmpty() && !style_val.endsWith(';'))
                style_val += ';';

            // keep trimmed style (without width/height)
            QString replacement = QStringLiteral("style=%1%2%1").arg(quote, style_val);
            // replace whole old style occurrence with cleaned version
            new_attrs.replace(style_m.captured(0), replacement);
        }

        // 3) ensure single leading space (attributes string may or may not start with space)
        new_attrs = new_attrs.trimmed();
        if (!new_attrs.isEmpty())
            new_attrs = " " + new_attrs;

        // 4) append width attribute
        new_attrs += QStringLiteral(" height=\"%1\"").arg(max_height_px);

        // reconstruct tag
        QString new_img = QStringLiteral("<img%1>").arg(new_attrs);
        result += new_img;

        last_pos = end;
    }

    // append remainder
    result += html.mid(last_pos);
    return result;
}

// Public method
QStringList HtmlHelper::extract_mathjax(const QString &html)
{
    QString work = html;         // mutable copy for masking
    QStringList results;

    auto capture_and_mask = [&](const QRegularExpression &re) {
        QRegularExpressionMatchIterator it = re.globalMatch(work);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            if (!m.hasMatch()) continue;
            // capture the whole matched substring (with delimiters)
            QString matched = m.captured(0);
            if (!matched.isEmpty()) results.append(matched);

            // mask matched region so later regexes won't capture it again
            int start = m.capturedStart(0);
            int len = m.capturedLength(0);
            if (start >= 0 && len > 0)
                work.replace(start, len, QString(len, QChar(' ')));
        }
    };

    // DOTALL option allows matching across newlines
    QRegularExpression::PatternOptions dotall = QRegularExpression::DotMatchesEverythingOption;

    // $$ ... $$ (display)
    QRegularExpression rx_dollars(R"(\$\$(.+?)\$\$)",
                                  QRegularExpression::CaseInsensitiveOption | dotall);
    capture_and_mask(rx_dollars);

    // \[ ... \] (display)
    QRegularExpression rx_brackets(R"(\\\[(.+?)\\\])",
                                   QRegularExpression::CaseInsensitiveOption | dotall);
    capture_and_mask(rx_brackets);

    // \( ... \) (inline)
    QRegularExpression rx_parens(R"(\\\((.+?)\\\))",
                                 QRegularExpression::CaseInsensitiveOption | dotall);
    capture_and_mask(rx_parens);

    // $ ... $ (inline) — avoid matching escaped \$ and avoid opening $ followed by whitespace
    QRegularExpression rx_single_dollar(R"((?<!\\)\$(?!\s)(.+?)(?<!\\)\$)",
                                        QRegularExpression::CaseInsensitiveOption | dotall);
    capture_and_mask(rx_single_dollar);

    return results;
}

// Public method
QString HtmlHelper::replace_mathjax_to_images(const QString& html,
                                              const QString& text_color,
                                              int img_height)
{
    // 1) Extract mathjax fragments from the input HTML (preserves original delimiters).
    //    Assumes extract_mathjax is available and returns matches in order.
    QStringList entries = extract_mathjax(html);

    // Map original mathjax => replacement HTML (<img .../>)
    QMap<QString, QString> mapped_entries;

    // Helper: remove common math delimiters and trim whitespace
    auto strip_delimiters = [](const QString& s) -> QString {
        QString t = s;
        if (t.startsWith("$$") && t.endsWith("$$") && t.size() >= 4) {
            t = t.mid(2, t.size() - 4);
        } else if (t.startsWith("\\[") && t.endsWith("\\]") && t.size() >= 4) {
            t = t.mid(2, t.size() - 4);
        } else if (t.startsWith("\\(") && t.endsWith("\\)") && t.size() >= 4) {
            t = t.mid(2, t.size() - 4);
        } else if (t.startsWith("$") && t.endsWith("$") && t.size() >= 2) {
            t = t.mid(1, t.size() - 2);
        }
        return t.trimmed();
    };

    // Iterate unique entries and build mapped replacements
    for (const QString& entry : entries) {
        if (entry.isEmpty()) continue;
        if (mapped_entries.contains(entry)) continue; // already processed

        // 2) Strip delimiters to get raw LaTeX
        QString latex = strip_delimiters(entry);
        latex = QUrl::toPercentEncoding(latex);

        // Optionally: if html contains HTML entities inside math, consider decoding them here.
        // For now we keep latex as-is (user wanted original MathJax → minimal changes).
        QString color = text_color;
        color.remove("#");

        const QString postfix = QString("&color=%1&scale=1").arg(color);

        // Compose final image URL for codecogs
        QString img_src = QStringLiteral("https://mathpad.ai/api/v1/latex2image?latex=%1")
                              .arg(latex + postfix);

        qDebug() << img_src, latex + postfix;

        // 4) Build the <img> HTML. Use height attribute as requested; width will be automatic (keep aspect).
        // You can later tweak style (e.g. style="max-width:100%;height:auto;") if needed in QML side.
        QString img_html = QStringLiteral("<img height=\"%1\" src=\"%2\"/>").arg(img_height).arg(img_src);

        mapped_entries.insert(entry, img_html);
    }

    // 5) Replace all occurrences in the original HTML.
    // We do a simple replace; for heavy duty HTML manipulations consider using an HTML parser.
    QString result = html;
    for (auto it = mapped_entries.constBegin(); it != mapped_entries.constEnd(); ++it) {
        // replace all occurrences of the exact MathJax fragment with the img HTML
        result.replace(it.key(), it.value());
    }

    return result;
}

// Public method
bool HtmlHelper::has_mathjax(const QString& html)
{
    return !extract_mathjax(html).empty();
}

} // namespace revise
