let quill = null;

function installMathJaxKatexShim(mathJax) {
    window.katex = {
        render(tex, element) {
            element.replaceChildren();

            try {
                const svg = mathJax.tex2svg(tex, { display: false });
                element.appendChild(svg);
            } catch (error) {
                element.textContent = tex;
            }
        },

        renderToString(tex) {
            try {
                return mathJax.tex2svg(tex, { display: false }).outerHTML;
            } catch (error) {
                return tex;
            }
        }
    };
}

function normalizeFormulaValue(value) {
    let text = String(value ?? "").trim();

    if (text.startsWith("\\(") && text.endsWith("\\)")) {
        text = text.slice(2, -2).trim();
    } else if (text.startsWith("\\[") && text.endsWith("\\]")) {
        text = text.slice(2, -2).trim();
    }

    return text;
}

function serializeHtmlWithMathJax(rootHtml) {
    const container = document.createElement("div");
    container.innerHTML = rootHtml;

    const formulas = container.querySelectorAll(".ql-formula");

    formulas.forEach((formula) => {
        const latex = normalizeFormulaValue(formula.getAttribute("data-value"));
        const textNode = document.createTextNode(`\\(${latex}\\)`);
        formula.replaceWith(textNode);
    });

    return container.innerHTML;
}

document.addEventListener("DOMContentLoaded", async function () {
    if (!window.MathJax) {
        throw new Error("MathJax is not loaded");
    }

    if (window.MathJax.startup?.promise) {
        await window.MathJax.startup.promise;
    }

    installMathJaxKatexShim(window.MathJax);

    const Delta = Quill.import("delta");

    const toolbarOptions = [
        [{ header: 1 }, { header: 2 }],
        ["bold", "italic", "underline", "strike"],
        [{ script: "sub" }, { script: "super" }],
        [{ list: "ordered" }, { list: "bullet" }],
        [{ align: [] }],
        ["blockquote", "code-block", "formula"],
    ];

    quill = new Quill("#editor", {
        theme: "snow",
        placeholder: "Введите текст...",
        modules: {
            toolbar: toolbarOptions,
            syntax: true,
        },
    });

    function textToDelta(text) {
        const delta = new Delta();
        const regex = /\\\((.*?)\\\)|\\\[(.*?)\\\]/gs;

        let lastIndex = 0;

        for (const match of text.matchAll(regex)) {
            const index = match.index ?? 0;

            if (index > lastIndex) {
                delta.insert(text.slice(lastIndex, index));
            }

            const latex = (match[1] ?? match[2] ?? "").trim();
            if (latex.length > 0) {
                delta.insert({ formula: latex });
            }

            lastIndex = index + match[0].length;
        }

        if (lastIndex < text.length) {
            delta.insert(text.slice(lastIndex));
        }

        return delta;
    }

    quill.root.addEventListener("paste", function (e) {
        const clipboard = e.clipboardData;
        if (!clipboard) return;

        const items = clipboard.items;

        for (let i = 0; i < items.length; i++) {
            const item = items[i];

            if (item.type.startsWith("image/")) {
                e.preventDefault();
                e.stopPropagation();
                e.stopImmediatePropagation();
                return false;
            }
        }
    }, true);

    quill.clipboard.addMatcher("IMG", () => new Delta());

    quill.clipboard.addMatcher(Node.TEXT_NODE, (node, delta) => {
        const text = node.data;

        if (!text.includes("\\(") && !text.includes("\\[")) {
            return delta;
        }

        return textToDelta(text);
    });

    window.editor = {
        getText: function () {
            return quill.getText();
        },

        setText: function (text) {
            quill.setText(text);
        },

        getHtml: function () {
            return serializeHtmlWithMathJax(quill.root.innerHTML);
        },

        setHtml: function (html) {
            quill.clipboard.dangerouslyPasteHTML(html);
        },

        clear: function () {
            quill.setText("");
        }
    };
});
