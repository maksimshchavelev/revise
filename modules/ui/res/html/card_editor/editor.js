let quill = null;


document.addEventListener("DOMContentLoaded", function () {
    const toolbarOptions = [
        [{ 'header': 1 }, { 'header': 2 }],
        ['bold', 'italic', 'underline', 'strike'],
        [{ 'script': 'sub'}, { 'script': 'super' }],
        [{ 'list': 'ordered'}, { 'list': 'bullet' }],
        [{ 'align': [] }],
        ['blockquote', 'code-block', 'formula'],

    ];
    
    quill = new Quill('#editor', {
        theme: 'snow',
        placeholder: 'Введите текст...',
        modules: {
            toolbar: toolbarOptions,
            syntax: true
        }
    });

    window.editor = {
        getText: function() {
            return quill.getText()
        },

        setText: function(text) {
            quill.setText(text)
        },

        getHtml: function() {
            return quill.root.innerHTML;
        },
        
        setHtml: function(html) {
            quill.root.innerHTML = html
        }
    }
});

