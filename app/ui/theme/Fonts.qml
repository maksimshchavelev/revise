pragma Singleton
import QtQuick

QtObject {
    property FontLoader regular: FontLoader {
        source: "qrc:/res/fonts/anonymous_pro/AnonymousPro-Regular.ttf"
    }

    property FontLoader italic: FontLoader {
        source: "qrc:/res/fonts/anonymous_pro/AnonymousPro-Italic.ttf"
    }

    property FontLoader bold: FontLoader {
        source: "qrc:/res/fonts/anonymous_pro/AnonymousPro-Bold.ttf"
    }

    property FontLoader boldItalic: FontLoader {
        source: "qrc:/res/fonts/anonymous_pro/AnonymousPro-BoldItalic.ttf"
    }
}
