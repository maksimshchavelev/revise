// Toast.qml

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype Toast
    \brief A temporary, animated notification popup.
    \inqmlmodule Revise
    \inherits Rectangle
    \since 1.0

    Toast displays a short message with a header, body text, and an optional
    icon. It slides in from the right edge of its parent, stays visible for a
    specified \l duration, and then slides back out. The toast can also be
    closed manually by the user.

    The toast blocks mouse clicks through its area while visible. Its position
    is controlled by the \l rightMargin property, and the enter/exit animations
    are handled internally.

    \section1 Example Usage

    \qml
    Revise.Toast {
        id: notificationToast
        header: qsTr("Success")
        text: qsTr("Your changes have been saved.")
        duration: 3000
        onClosed: console.log("Toast closed")

        Component.onCompleted: open()
    }
    \endqml
*/
Rectangle {
    id: root

    /*!
        \qmlproperty string Toast::header
        \brief The title text displayed at the top of the toast.

        The text is bold, word-wrapped, and limited to two lines.
    */
    property string header

    /*!
        \qmlproperty string Toast::text
        \brief The main body text displayed below the header.

        The text is word-wrapped and limited to ten lines.
    */
    property string text

    /*!
        \qmlproperty int Toast::rightMargin
        \brief The distance in pixels from the right edge of the parent.
        \default 16

        This value defines the final resting position of the toast when it is
        fully opened.
    */
    property int rightMargin: 16

    /*!
        \qmlproperty int Toast::duration
        \brief The time in milliseconds before the toast closes automatically.
        \required

        Setting this property is mandatory. If the user does not close the
        toast manually, it will start closing after this interval.
    */
    required property int duration

    /*!
        \qmlproperty url Toast::icon
        \brief The source URL of the icon displayed on the left side.

        This property is an alias to the source of the internal \l Revise.Icon.
    */
    property alias icon: icon.source

    /*!
        \qmlsignal Toast::opened()
        \brief Emitted when the toast starts its opening animation.
    */
        signal opened

    /*!
        \qmlsignal Toast::closed()
        \brief Emitted after the closing animation finishes and the toast is hidden.
    */
        signal closed

    width: 340
    height: rowLayout.implicitHeight + 30
    radius: 10
    visible: false
    color: Revise.Theme.toastSuccess

    layer.effect: Revise.Shadow {}

    // Block clicks through toasts
    MouseArea {
        anchors.fill: parent
    }

    RowLayout {
        id: rowLayout
        spacing: 10

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: 10
        }

        Item {
            Layout.preferredWidth: 36
            Layout.preferredHeight: 36

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.3
                radius: width * 0.3
            }

            Revise.Icon {
                id: icon
                anchors.fill: parent
                anchors.margins: 4
                source: "qrc:/res/img/success.svg"
            }
        }

        ColumnLayout {
            id: columnLayout

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4

            // Header
            Revise.Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.header
                font.bold: true
                font.pixelSize: Revise.Theme.fontSizeMedium
                horizontalAlignment: Text.AlignLeft
                maximumLineCount: 2
                color: "black"
                opacity: 0.75
                wrapMode: Text.WordWrap
            }

            // Message
            Revise.Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.text
                horizontalAlignment: Text.AlignLeft
                color: "black"
                maximumLineCount: 10
                opacity: 0.75
                wrapMode: Text.WordWrap
            }
        }

        Revise.IconButton {
            id: closeButton
            Layout.preferredWidth: 28
            Layout.preferredHeight: 28
            icon.source: "qrc:/res/img/close.svg"
            background.color: Color.darker(root.color, 1.1)
            icon.anchors.margins: 6
            icon.color: Revise.Theme.foregroundColor
            onClicked: root.close()
        }

        Item {}
    }

    property NumberAnimation openAnim: NumberAnimation {
        target: root
        property: "x"
        duration: 300
        easing.type: Easing.OutCubic
    }

    property NumberAnimation closeAnim: NumberAnimation {
        target: root
        property: "x"
        duration: 300
        easing.type: Easing.InCubic
        onFinished: {
            root.visible = false
            root.closed()
        }
    }

    Timer {
        id: closeTimer
        interval: root.duration
        running: false
        onTriggered: root.close()
    }

    /*!
        \qmlmethod void Toast::open()
        \brief Shows the toast and starts the opening animation.

        The toast is positioned just outside the right edge of its parent and
        animates into its final position defined by \l rightMargin. It also
        starts the timer for the automatic closing based on \l duration.
    */
    function open() {
        if (!parent)
            return

        root.visible = true
        root.x = parent.width

        openAnim.to = parent.width - width - root.rightMargin
        openAnim.start()

        closeTimer.start()
        opened()
    }

    /*!
        \qmlmethod void Toast::close()
        \brief Hides the toast and starts the closing animation.

        The timer for automatic closing is stopped, and the toast animates out
        to the right edge of its parent. The \l closed signal is emitted when
        the animation finishes.
    */
    function close() {
        if (!parent)
            return

        closeAnim.to = parent.width
        closeAnim.start()
        closeTimer.stop()
    }
}
