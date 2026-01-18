// ValidatedMultilineTextField.qml
import QtQuick
import QtQuick.Controls
import "qrc:/ui/theme"

Item {
    id: root

    // Public properties
    property alias text: textInput.text
    property alias placeholderText: placeholder.text
    property alias inputMethodHints: textInput.inputMethodHints
    property bool valid: true
    property bool editable: true
    property Item outerFlickable: null
    property TextEdit rawTextEdit: textInput

    // Signals
    signal editingFinished
    signal accepted

    // Visual properties
    property int borderWidth: 2
    property color defaultBorderColor: Theme.textInputDefaultBorder
    property color validBorderColor: Theme.textInputValidBorder
    property color invalidBorderColor: Theme.textInputInvalidBorder
    property color backgroundColor: "black"
    property color textColor: Theme.textColor
    property color placeholderColor: Theme.textColorDark
    property int fontSize: Theme.textSizeDefault
    property int padding: 8

    // Private state: current border color
    readonly property color currentBorderColor: {
        if (textInput.length === 0) {
            return defaultBorderColor
        } else if (valid) {
            return validBorderColor
        } else {
            return invalidBorderColor
        }
    }

    // Component basic size
    width: 200
    implicitHeight: 120

    // Background rectangle
    Rectangle {
        anchors.fill: parent
        radius: 4
        color: backgroundColor
        border.width: borderWidth
        border.color: currentBorderColor
        opacity: 0.4
    }

    Item {
        id: contentContainer
        anchors.fill: parent
        anchors.leftMargin: padding
        anchors.rightMargin: padding
        anchors.topMargin: padding
        anchors.bottomMargin: padding

        // Placeholder text (visible when empty and not focused)
        Text {
            id: placeholder
            anchors.fill: parent
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            color: placeholderColor
            font.pixelSize: fontSize
            visible: textInput.text.length === 0 && !textInput.activeFocus
            elide: Text.ElideRight
        }

        // ScrollView with a custom Flickable contentItem.
        // We don't set ScrollBar properties (we leave default / automatic).
        ScrollView {
            id: scrollView
            anchors.fill: parent
            clip: true

            // Provide our own Flickable so we can control contentHeight/contentY precisely.
            contentItem: Flickable {
                id: innerFlick
                width: parent.width
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                // Keep contentWidth consistent to prevent horizontal scrolling.
                contentWidth: width

                // TextEdit is the only child and grows vertically via implicitHeight.
                TextEdit {
                    id: textInput
                    width: innerFlick.width
                    // Do not set height: allow implicitHeight to grow and Flickable to scroll.
                    wrapMode: TextEdit.WordWrap
                    verticalAlignment: TextEdit.AlignTop
                    horizontalAlignment: TextEdit.AlignLeft
                    readOnly: !root.editable

                    // Visuals
                    color: textColor
                    font.pixelSize: fontSize

                    Keys.onPressed: {
                        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                            event.accepted = true
                            focus = false
                            root.accepted()
                        }
                    }

                    onEditingFinished: {
                        root.editingFinished()
                        focus = false
                    }

                    onActiveFocusChanged: {
                        // Disable outer flickable (if provided) to avoid gesture conflicts.
                        if (root.outerFlickable !== null) {
                            root.outerFlickable.interactive = !activeFocus
                        }
                        // When we gain focus, ensure the cursor is visible (keyboard might appear).
                        if (activeFocus) {
                            // schedule call to ensure layout settled
                            Qt.callLater(function() { ensureCursorVisible(cursorRectangle) })
                        }
                    }

                    // Keep Flickable contentHeight in sync with actual text height.
                    onImplicitHeightChanged: {
                        innerFlick.contentHeight = implicitHeight
                        // If focused, ensure cursor visible after height change (new line added).
                        if (activeFocus) {
                            Qt.callLater(function() { ensureCursorVisible(cursorRectangle) })
                        }
                    }

                    // When cursor rectangle changes (typing / moving), ensure it's visible.
                    onCursorRectangleChanged: {
                        // schedule to avoid racing with layout updates
                        Qt.callLater(function() { ensureCursorVisible(cursorRectangle) })
                    }

                    // Helper: expose small wrapper to call ensureCursorVisible from other places.
                    function ensureCursorVisible(rect) {
                        // English comments only inside code as requested.
                        if (!rect) return;

                        // padding in px to keep some space around the cursor
                        var padding = 8;

                        // Compute visible window
                        var viewTop = innerFlick.contentY;
                        var viewBottom = innerFlick.contentY + innerFlick.height;

                        // Cursor rectangle is in TextEdit coordinates.
                        // We use its y and height directly.
                        var cursorTop = rect.y;
                        var cursorBottom = rect.y + rect.height;

                        // If the content fits entirely, nothing to do.
                        if (innerFlick.contentHeight <= innerFlick.height) {
                            // small attempt to keep cursor visible near center if keyboard overlaps
                            innerFlick.contentY = 0;
                            return;
                        }

                        // Scroll up: cursor above viewport
                        if (cursorTop < viewTop + padding) {
                            var target = Math.max(0, cursorTop - padding);
                            innerFlick.contentY = target;
                            return;
                        }

                        // Scroll down: cursor below viewport
                        if (cursorBottom > viewBottom - padding) {
                            // ensure we don't overflow maximum contentY
                            var maxContentY = Math.max(0, innerFlick.contentHeight - innerFlick.height);
                            var target = Math.min(maxContentY, cursorBottom - innerFlick.height + padding);
                            innerFlick.contentY = target;
                            return;
                        }

                        // Cursor already visible — nothing to do.
                    }
                }

                // Small MouseArea to focus TextEdit on tap inside Flickable area.
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        textInput.forceActiveFocus()
                    }
                    // prevent it from stealing wheel events etc. let TextEdit receive them when focused.
                    hoverEnabled: false
                }
            } // end contentItem Flickable
        } // end ScrollView
    } // end contentContainer

    // Read-only overlay when editable == false
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.4
        visible: !root.editable
        radius: 4

        MouseArea {
            anchors.fill: parent
        }
    }
}
