// Statistics page
import QtQuick
import QtQuick.Layouts
import "../views/training"
import "../controls"
import "../theme"

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        AppText {
            text: qsTr("Статистика")
            font.pixelSize: Theme.textSizeBig
            Layout.alignment: Qt.AlignHCenter
        }

        CardView {
            front: "Hello sdkjglksfjg lkdfj glkdsfjkl gjdslk"
            Layout.fillWidth: true
        }

        Image {
            width: 50
            height: 25
            source: "image://math/\\frac{a^2+b^2}{c}"
            fillMode: Image.PreserveAspectFit
        }
    }
}
