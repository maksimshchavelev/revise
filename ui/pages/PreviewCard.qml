// Card preview page
import QtQuick
import QtQuick.Layouts
import "../theme"
import "../controls"

Item {
    id: root

    // front of the card
    property string front
    // back of the card
    property string back
    // flipped?
    property bool flipped: false
    // Previous page
    property int previousPageIndex: 0

    signal backClicked(int previousPageIndex)

    function exitClicked() {
        root.flipped = false
        root.backClicked(root.previousPageIndex)
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.2
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        Flickable {
            id: flickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8
            contentWidth: width
            contentHeight: Math.max(text.implicitHeight, height)
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: htmlHelper.has_mathjax(
                                    text) ? Flickable.HorizontalFlick : Flickable.VerticalFlick

            AppText {
                id: text
                width: parent.width
                anchors.centerIn: parent
                font.pixelSize: Theme.textSizeMedium
                wrapMode: htmlHelper.has_mathjax(
                              text) ? Text.NoWrap : Text.WordWrap
                text: {
                    var prepared_text = htmlHelper.scale_images_to_width(
                                root.flipped ? root.back : root.front, parent.width)
                    prepared_text = htmlHelper.replace_mathjax_to_images(
                                prepared_text, Theme.textColor,
                                Theme.textSizeMedium)
                    return prepared_text
                }

                textFormat: Text.RichText
                horizontalAlignment: htmlHelper.has_mathjax(
                                         text) ? Text.AlignLeft : Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Перевернуть")
            onClicked: root.flipped = !root.flipped
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Назад")
            onClicked: {
                root.flipped = false
                root.backClicked(root.previousPageIndex)
            }
        }
    }
}
