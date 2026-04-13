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
        anchors.bottom: root.bottom
    }

    Flickable {
        anchors {
            left: root.left
            top: root.top
            bottom: toolbar.top
            right: root.right
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
                        text: settings.maxInterval
                        inputMethodHints: Qt.ImhDigitsOnly
                        background.border.color: valid ? Revise.Theme.textFieldBorder : Revise.Theme.red

                        validatorFunction: function (text) {
                            const value = parseInt(text)
                            return 1 <= value && value <= 365
                        }

                        validator: IntValidator {
                            bottom: 1
                            top: 365
                        }

                        onTextChanged: {
                            if (!valid)
                                return

                            settings.maxInterval = parseInt(text)
                        }
                    }
                }

                // learning speed
                Revise.SettingsRow {
                    text: qsTr("Скорость обучения")

                    Revise.TextField {
                        Layout.fillWidth: true
                        placeholder.text: qsTr("От 1 до 5")
                        text: settings.learningRate.toFixed(1)
                        inputMethodHints: Qt.ImhDigitsOnly
                        background.border.color: valid ? Revise.Theme.textFieldBorder : Revise.Theme.red

                        validatorFunction: function (text) {
                            const value = parseFloat(text)
                            return 1 <= value && value <= 5
                        }

                        validator: DoubleValidator {
                            locale: "en_US"
                            notation: DoubleValidator.StandardNotation
                            bottom: 1.0
                            top: 5.0
                        }

                        onTextChanged: {
                            if (!valid)
                                return

                            text = text.replace(",", ".")
                            settings.learningRate = parseFloat(text)
                        }
                    }
                }
            }
        }
    }
}
