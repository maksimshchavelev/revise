// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "utils/Html.hpp"                  // for header
#include <QRegularExpressionMatchIterator> // for QRegularExpressionMatchIterator

namespace utils {

QString Html::replace_images_src(const QString& html, const QMap<QString, QString>& mapped_images) {
    if (mapped_images.isEmpty()) {
        return html;
    }

    QString result = html;

    // Matches: <img ... src="value" ...> or <img ... src='value' ...>
    static const QRegularExpression img_src_regex(R"(<img[^>]+src\s*=\s*(['"])([^'"]+)\1)",
                                                  QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator it = img_src_regex.globalMatch(result);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        const QString           full_match = match.captured(0);
        const QString           quote = match.captured(1);
        const QString           src_value = match.captured(2);

        auto map_it = mapped_images.find(src_value);
        if (map_it == mapped_images.end()) {
            continue;
        }

        const QString new_src = *map_it;

        QString replaced = full_match;
        replaced.replace(QString("src=%1%2%1").arg(quote, src_value), QString("src=%1%2%1").arg(quote, new_src));

        result.replace(full_match, replaced);
    }

    return result;
}

} // namespace utils
