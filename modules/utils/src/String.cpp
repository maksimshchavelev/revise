// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "utils/String.hpp"   // for header
#include <QRegularExpression> // for QRegularExpression

namespace utils {

QString String::replace_katex(const QString& input) {
    static const QHash<QString, QString> map = {{"\\(", ""},
                                                {"\\)", ""},
                                                {"\\left", ""},
                                                {"\\right", ""},
                                                {"\\middle", ""},
                                                {"\\times", "×"},
                                                {"\\cdot", "·"},
                                                {"\\div", "÷"},
                                                {"\\pm", "±"},
                                                {"\\mp", "∓"},
                                                {"\\neq", "≠"},
                                                {"\\ne", "≠"},
                                                {"\\leq", "≤"},
                                                {"\\le", "≤"},
                                                {"\\geq", "≥"},
                                                {"\\ge", "≥"},
                                                {"\\approx", "≈"},
                                                {"\\equiv", "≡"},
                                                {"\\sim", "~"},
                                                {"\\mid", "|"},
                                                {"\\nmid", "∤"},

                                                {"\\sin", "sin"},
                                                {"\\cos", "cos"},
                                                {"\\tan", "tg"},
                                                {"\\tg", "tg"},
                                                {"\\cot", "ctg"},
                                                {"\\ctg", "ctg"},
                                                {"\\sec", "sec"},
                                                {"\\csc", "csc"},
                                                {"\\arcsin", "arcsin"},
                                                {"\\arccos", "arccos"},
                                                {"\\arctan", "arctg"},
                                                {"\\arctg", "arctg"},
                                                {"\\arccot", "arcctg"},
                                                {"\\arcctg", "arcctg"},

                                                {"\\sinh", "sh"},
                                                {"\\cosh", "ch"},
                                                {"\\tanh", "th"},
                                                {"\\coth", "cth"},

                                                {"\\log", "log"},
                                                {"\\ln", "ln"},
                                                {"\\lg", "lg"},
                                                {"\\exp", "exp"},
                                                {"\\lim", "lim"},
                                                {"\\limsup", "lim sup"},
                                                {"\\liminf", "lim inf"},
                                                {"\\max", "max"},
                                                {"\\min", "min"},
                                                {"\\sup", "sup"},
                                                {"\\inf", "inf"},
                                                {"\\det", "det"},
                                                {"\\dim", "dim"},
                                                {"\\gcd", "НОД"},
                                                {"\\mod", "mod"},

                                                {"\\sum", "∑"},
                                                {"\\prod", "∏"},
                                                {"\\coprod", "∐"},
                                                {"\\int", "∫"},
                                                {"\\iint", "∬"},
                                                {"\\iiint", "∭"},
                                                {"\\oint", "∮"},
                                                {"\\sqrt", "√"},
                                                {"\\infty", "∞"},
                                                {"\\partial", "∂"},
                                                {"\\nabla", "∇"},

                                                {"\\to", "→"},
                                                {"\\rightarrow", "→"},
                                                {"\\leftarrow", "←"},
                                                {"\\leftrightarrow", "↔"},
                                                {"\\uparrow", "↑"},
                                                {"\\downarrow", "↓"},
                                                {"\\updownarrow", "↕"},
                                                {"\\Rightarrow", "⇒"},
                                                {"\\Leftarrow", "⇐"},
                                                {"\\Leftrightarrow", "⇔"},
                                                {"\\Uparrow", "⇑"},
                                                {"\\Downarrow", "⇓"},
                                                {"\\Updownarrow", "⇕"},
                                                {"\\nearrow", "↗"},
                                                {"\\searrow", "↘"},
                                                {"\\swarrow", "↙"},
                                                {"\\nwarrow", "↖"},

                                                {"\\degree", "°"},
                                                {"\\angle", "∠"},
                                                {"\\triangle", "△"},
                                                {"\\perp", "⊥"},
                                                {"\\parallel", "∥"},
                                                {"\\in", "∈"},
                                                {"\\notin", "∉"},
                                                {"\\subset", "⊂"},
                                                {"\\subseteq", "⊆"},
                                                {"\\cup", "∪"},
                                                {"\\cap", "∩"},
                                                {"\\emptyset", "∅"},
                                                {"\\varnothing", "∅"},

                                                {"\\forall", "∀"},
                                                {"\\exists", "∃"},
                                                {"\\nexists", "∄"},
                                                {"\\neg", "¬"},
                                                {"\\lnot", "¬"},
                                                {"\\overline", "¬"},
                                                {"\\land", "∧"},
                                                {"\\wedge", "∧"},
                                                {"\\lor", "∨"},
                                                {"\\vee", "∨"},
                                                {"\\oplus", "⊕"},
                                                {"\\veebar", "⊻"},
                                                {"\\implies", "⇒"},
                                                {"\\iff", "⇔"},
                                                {"\\top", "⊤"},
                                                {"\\bot", "⊥"},
                                                {"\\vdash", "⊢"},
                                                {"\\models", "⊨"},

                                                {"\\alpha", "α"},
                                                {"\\beta", "β"},
                                                {"\\gamma", "γ"},
                                                {"\\delta", "δ"},
                                                {"\\epsilon", "ε"},
                                                {"\\varepsilon", "ε"},
                                                {"\\zeta", "ζ"},
                                                {"\\eta", "η"},
                                                {"\\theta", "θ"},
                                                {"\\vartheta", "ϑ"},
                                                {"\\iota", "ι"},
                                                {"\\kappa", "κ"},
                                                {"\\lambda", "λ"},
                                                {"\\mu", "μ"},
                                                {"\\nu", "ν"},
                                                {"\\xi", "ξ"},
                                                {"\\pi", "π"},
                                                {"\\varpi", "ϖ"},
                                                {"\\rho", "ρ"},
                                                {"\\varrho", "ϱ"},
                                                {"\\sigma", "σ"},
                                                {"\\varsigma", "ς"},
                                                {"\\tau", "τ"},
                                                {"\\upsilon", "υ"},
                                                {"\\phi", "φ"},
                                                {"\\varphi", "ϕ"},
                                                {"\\chi", "χ"},
                                                {"\\psi", "ψ"},
                                                {"\\omega", "ω"},

                                                {"\\Gamma", "Γ"},
                                                {"\\Delta", "Δ"},
                                                {"\\Theta", "Θ"},
                                                {"\\Lambda", "Λ"},
                                                {"\\Xi", "Ξ"},
                                                {"\\Pi", "Π"},
                                                {"\\Sigma", "Σ"},
                                                {"\\Upsilon", "Υ"},
                                                {"\\Phi", "Φ"},
                                                {"\\Psi", "Ψ"},
                                                {"\\Omega", "Ω"},

                                                {"\\big", ""},
                                                {"\\Big", ""},
                                                {"\\bigg", ""},
                                                {"\\Bigg", ""},
                                                {"\\bigl", ""},
                                                {"\\bigr", ""},
                                                {"\\Bigl", ""},
                                                {"\\Bigr", ""},
                                                {"\\biggl", ""},
                                                {"\\biggr", ""},
                                                {"\\Biggl", ""},
                                                {"\\Biggr", ""},

                                                {"\\langle", "⟨"},
                                                {"\\rangle", "⟩"},
                                                {"\\lvert", "|"},
                                                {"\\rvert", "|"},
                                                {"\\lVert", "‖"},
                                                {"\\rVert", "‖"},
                                                {"\\vert", "|"},
                                                {"\\Vert", "‖"},
                                                {"\\lfloor", "⌊"},
                                                {"\\rfloor", "⌋"},
                                                {"\\lceil", "⌈"},
                                                {"\\rceil", "⌉"},
                                                {"\\lbrace", "{"},
                                                {"\\rbrace", "}"},
                                                {"\\lbrack", "["},
                                                {"\\rbrack", "]"},
                                                {"\\backslash", "\\"},

                                                {"\\{", "{"},
                                                {"\\}", "}"},
                                                {"\\|", "‖"}};

    static const QRegularExpression textWrapperRe(R"(\\(?:text|mathrm|mathbf|mathit|ce)\{([^{}]*)\})");
    QString processed = input;
    processed.replace(textWrapperRe, "\\1");

    processed.replace("\\left.", "");
    processed.replace("\\right.", "");
    processed.replace("\\middle.", "");

    static const QRegularExpression re(R"(\\\(|\\\)|\\[a-zA-Z]+|\\[{}|])");

    QString result;
    result.reserve(processed.size());
    int lastPos = 0;

    for (const QRegularExpressionMatch& match : re.globalMatch(processed)) {
        result.append(QStringView(processed).mid(lastPos, match.capturedStart() - lastPos));

        QString token = match.captured();
        auto    it = map.constFind(token);
        result.append(it != map.constEnd() ? it.value() : token);

        lastPos = match.capturedEnd();
    }

    result.append(QStringView(processed).mid(lastPos));
    return result;
}

} // namespace utils
