#include <utils/Html.hpp>
#include <gtest/gtest.h>

using namespace utils;

// Center vertical

TEST(HtmlTest, center_vertical_empty_html) {
    QString res = Html::center_vertical("");

    ASSERT_TRUE(res.isEmpty());
}

TEST(HtmlTest, center_vertical_html) {
    QString res = Html::center_vertical("<p>Hello world</p>");
    QString expected = R"(
        <div style="display: flex; flex-direction: column; justify-content: center; min-height: 100vh;">
           <div style="text-align: left;"><p>Hello world</p></div>
        </div>
    )";

    ASSERT_EQ(res, expected);
}

// Center horizontal

TEST(HtmlTest, center_horizontal_empty_html) {
    QString res = Html::center_horizontal("");

    ASSERT_TRUE(res.isEmpty());
}

TEST(HtmlTest, center_horizontal_html) {
    QString res = Html::center_horizontal("<p>Hello world</p>");
    QString expected = R"(
        <div style="text-align: center;">
            <p>Hello world</p>
        </div>
    )";

    ASSERT_EQ(res, expected);
}

// New lines

TEST(HtmlTest, new_lines_empty_html) {
    std::size_t res = Html::new_lines("");

    ASSERT_EQ(res, 0);
}

TEST(HtmlTest, new_lines_html) {
    std::size_t res = Html::new_lines(R"(
        <html>
            <head>
                <title>Test Document</title>
            </head>
            <body>
                <h1>Welcome to the Test Page</h1>

                <p>This is a simple paragraph with <b>bold text</b> and <i>italic text</i>.</p>

                <div>
                    <p>Another paragraph inside a div.<br>This line has a line break.</p>
                    <p>Third paragraph with <span style="color: red;">colored text</span>.</p>
                </div>

                <ul>
                    <li>First item</li>
                    <li>Second item<br>with break inside</li>
                    <li>Third item</li>
                </ul>

                <p>Final paragraph with <a href="https://example.com">a link</a> and <strong>strong text</strong>.</p>
            </body>
        </html>
    )");

    ASSERT_EQ(res, 10);
}

