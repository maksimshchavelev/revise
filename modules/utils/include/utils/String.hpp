// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>  // for QString

namespace utils {

/// @brief Utility functions for working with strings
class String {
public:
    String() = delete;

    /// @brief Replace some KaTeX tags with regular characters, if possible and remove open and close tags
    /// @param input Input string
    /// @return Formatted string without some KaTeX tags
    [[nodiscard]] static QString replace_katex(const QString& input);
};

} // namespace utils
