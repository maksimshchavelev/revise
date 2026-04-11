import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Revise as Revise

Item {
    id: root

    property var pageParams: null
    property string windowTitle: qsTr("Настройки - Revise")
    property bool openedAsWindow: false

    Revise.Background {
        id: background
        anchors.fill: parent
    }

    Revise.BlurOverlay {
        id: blurOverlay
        anchors.fill: parent
        sourceItem: background
        blur: 1.4
    }

    Revise.Toolbar {
        id: toolbar
        height: parent.height
    }

    Flickable {
        anchors {
            left: toolbar.right
            top: root.top
            bottom: root.bottom
            right: root.right
            margins: 12
        }

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: layout.implicitHeight

        ColumnLayout {
            id: layout
            width: parent.width
            spacing: 12

            Revise.SettingsBlock {
                header.text: qsTr("Алгоритм обучения")

                // max interval
                Revise.SettingsRow {
                    text: qsTr("Максимальный интервал (дней):")

                    Revise.TextField {
                        Layout.fillWidth: true
                        placeholder.text: qsTr("От 1 до 365")
                        inputMethodHints: Qt.ImhDigitsOnly
                        background.border.color: valid ? Revise.Theme.textFieldBorder : Revise.Theme.red

                        validatorFunction: function(text) {
                            const value = parseInt(text)
                            return 1 <= value && value <= 365
                        }

                        validator: IntValidator {
                            bottom: 1
                            top: 365
                        }
                    }
                }

                // learning speed
                Revise.SettingsRow {
                    text: qsTr("Скорость обучения")

                    Revise.TextField {
                        Layout.fillWidth: true
                        placeholder.text: qsTr("От 1 до 5")
                        inputMethodHints: Qt.ImhDigitsOnly
                        background.border.color: valid ? Revise.Theme.textFieldBorder : Revise.Theme.red

                        validatorFunction: function(text) {
                            const value = parseFloat(text)
                            return 1 <= value && value <= 5
                        }

                        validator: DoubleValidator {
                            bottom: 1.0
                            top: 5.0
                        }
                    }
                }
            }
        }
    }
}
