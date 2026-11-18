// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <gtest/gtest.h>    // for gtest
#include <utils/String.hpp> // for String

using namespace utils;

TEST(StringReplaceKatexTest, EmptyAndPlainText) {
    EXPECT_EQ(String::replace_katex(""), QString(""));
    EXPECT_EQ(String::replace_katex("A regular text without formula 123"), QString("A regular text without formula 123"));
}

TEST(StringReplaceKatexTest, DelimitersRemoval) {
    EXPECT_EQ(String::replace_katex(R"(\(x + y = 5\))"), QString("x + y = 5"));
    EXPECT_EQ(String::replace_katex(R"(Formula \(a + b\) and \(c - d\))"), QString("Formula a + b and c - d"));
    EXPECT_EQ(String::replace_katex(R"(\left(x + y\right))"), QString("(x + y)"));
}

TEST(StringReplaceKatexTest, BasicArithmeticAndComparisons) {
    EXPECT_EQ(String::replace_katex(R"(\(a \times b \cdot c \div d\))"), QString("a × b · c ÷ d"));
    EXPECT_EQ(String::replace_katex(R"(\(x \pm y \mp z\))"), QString("x ± y ∓ z"));
    EXPECT_EQ(String::replace_katex(R"(\(a \neq b, c \ne d, e \approx f, g \equiv h, i \sim j\))"),
              QString("a ≠ b, c ≠ d, e ≈ f, g ≡ h, i ~ j"));
}

TEST(StringReplaceKatexTest, PrefixConflictInequalities) {
    EXPECT_EQ(String::replace_katex(R"(\(x \leq y \le z\))"), QString("x ≤ y ≤ z"));
    EXPECT_EQ(String::replace_katex(R"(\(a \geq b \ge c\))"), QString("a ≥ b ≥ c"));
}

TEST(StringReplaceKatexTest, TrigonometricAndInverseFunctions) {
    EXPECT_EQ(String::replace_katex(R"(\(\sin x + \cos x + \tan x + \tg x\))"), QString("sin x + cos x + tg x + tg x"));
    EXPECT_EQ(String::replace_katex(R"(\(\cot x + \ctg x\))"), QString("ctg x + ctg x"));
    EXPECT_EQ(String::replace_katex(R"(\(\arcsin x + \arccos x + \arctan x + \arctg x\))"),
              QString("arcsin x + arccos x + arctg x + arctg x"));
    EXPECT_EQ(String::replace_katex(R"(\(\arccot x + \arcctg x\))"), QString("arcctg x + arcctg x"));
}

TEST(StringReplaceKatexTest, GreekAlphabet) {
    EXPECT_EQ(String::replace_katex(R"(\(\alpha + \beta + \gamma + \delta + \pi + \omega\))"), QString("α + β + γ + δ + π + ω"));
    EXPECT_EQ(String::replace_katex(R"(\(\varepsilon + \varphi + \vartheta\))"), QString("ε + ϕ + ϑ"));
    EXPECT_EQ(String::replace_katex(R"(\(\Gamma + \Delta + \Lambda + \Sigma + \Omega\))"), QString("Γ + Δ + Λ + Σ + Ω"));
}

TEST(StringReplaceKatexTest, AnalysisAndFunctions) {
    EXPECT_EQ(String::replace_katex(R"(\(\log_2 x + \ln y + \lg z\))"), QString("log_2 x + ln y + lg z"));
    EXPECT_EQ(String::replace_katex(R"(\(\lim_{x \to \infty} \sum_{i=1}^{n} \int f(x)\))"),
              QString("lim_{x → ∞} ∑_{i=1}^{n} ∫ f(x)"));
    EXPECT_EQ(String::replace_katex(R"(\(\sqrt{x} + \partial y + \nabla z\))"), QString("√{x} + ∂ y + ∇ z"));
}

TEST(StringReplaceKatexTest, LogicAndSets) {
    EXPECT_EQ(String::replace_katex(R"(\(\forall x \in A \exists y \notin B : x \subset y\))"),
              QString("∀ x ∈ A ∃ y ∉ B : x ⊂ y"));
    EXPECT_EQ(String::replace_katex(R"(\((A \land B) \lor \neg C \implies D \iff E\))"), QString("(A ∧ B) ∨ ¬ C ⇒ D ⇔ E"));
    EXPECT_EQ(String::replace_katex(R"(\(A \cup B \cap C = \emptyset\))"), QString("A ∪ B ∩ C = ∅"));
}

TEST(StringReplaceKatexTest, ArrowsAndDivisibility) {
    EXPECT_EQ(String::replace_katex(R"(\(a \mid b, c \nmid d, e \parallel f, g \perp h\))"),
              QString("a | b, c ∤ d, e ∥ f, g ⊥ h"));
    EXPECT_EQ(String::replace_katex(R"(\(\uparrow \downarrow \leftarrow \rightarrow \leftrightarrow\))"), QString("↑ ↓ ← → ↔"));
}

TEST(StringReplaceKatexTest, TextWrappersAndUnknownCommands) {
    EXPECT_EQ(String::replace_katex(R"(\(\text{Li}^{7}_{3} + \mathrm{He}^{4}_{2}\))"), QString("Li^{7}_{3} + He^{4}_{2}"));
    EXPECT_EQ(String::replace_katex(R"(\(\ce{H2O} \downarrow\))"), QString("H2O ↓"));
    EXPECT_EQ(String::replace_katex(R"(\(\unknowncmd{123} + \frac{1}{2}\))"), QString(R"(\unknowncmd{123} + \frac{1}{2})"));
}
