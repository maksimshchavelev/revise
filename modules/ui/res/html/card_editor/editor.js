let quill = null;

document.addEventListener("DOMContentLoaded", function () {
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

    /**
     * Convert text containing \( ... \) into a Delta with formula embeds.
     * KaTeX formulas stay as Quill embeds instead of plain text.
     *
     * @param {string} text
     * @returns {Delta}
     */
    function textToDelta(text) {
        const delta = new Delta();

        /**
         * Matches:
         *   \( ... \)
         * Optional support for \[ ... \] is also included.
         */
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

    quill.root.addEventListener('paste', function (e) {
        const clipboard = e.clipboardData;
        if (!clipboard) return;

        const items = clipboard.items;

        for (let i = 0; i < items.length; i++) {
            const item = items[i];

            if (item.type.startsWith('image/')) {
                e.preventDefault();
                e.stopPropagation();
                e.stopImmediatePropagation();
                return false;
            }
        }
    }, true);

    quill.clipboard.addMatcher('IMG', () => new Delta());

    /**
     * Replace text nodes with parsed math embeds during paste/import.
     */
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
            return quill.root.innerHTML;
        },

        setHtml: function (html) {
            quill.clipboard.dangerouslyPasteHTML(html);
        },

        clear: function() {
            quill.setText('');
        }
    };
});