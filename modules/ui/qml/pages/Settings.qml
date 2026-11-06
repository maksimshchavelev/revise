// Settings.qml

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

    Revise.Toolbar {
        id: toolbar
        height: parent.height
    }

    Revise.Flickable {
        anchors {
            left: toolbar.right
            top: root.top
            bottom: root.bottom
            right: root.right
            margins: 12
        }

        contentWidth: width
        contentHeight: layout.implicitHeight

        Column {
            id: layout
            width: parent.width - 16
            spacing: 12

            // Main application settings
            Revise.SettingsPanel {
                width: parent.width
                header.label.text: qsTr("Основные настройки")
                header.icon.source: "qrc:/res/img/settings.svg"

                Revise.SettingsRow {
                    text: qsTr("Масштаб интерфейса")

                    Row {
                        width: 180
                        spacing: 8

                        Revise.Text {
                            width: 30
                            id: uiScaleText
                            text: `${uiScaleSlider.value.toFixed(0)}%`
                            anchors.verticalCenter: uiScaleSlider.verticalCenter
                        }

                        Revise.Slider {
                            id: uiScaleSlider

                            width: parent.width - learningSpeedText.width - parent.spacing
                            from: 70
                            to: 130
                            stepSize: 5

                            Binding on value {
                                value: Revise.SettingsService.uiScale * 100
                                when: !uiScaleSlider.pressed
                            }

                            onMoved: Revise.SettingsService.uiScale = value / 100
                        }
                    }
                }
            }

            // Study algorithm settings
            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/action.svg"
                header.label.text: qsTr("Алгоритм обучения")

                // max learning interval
                Revise.SettingsRow {
                    text: qsTr("Максимальный интервал (дней)")

                    Revise.SpinBox {
                        id: maxIntervalSpinBox

                        width: 180
                        Layout.fillWidth: true
                        minimum: 1
                        maximum: 365

                        Binding on value {
                            value: Revise.SettingsService.maxInterval
                            when: !maxIntervalSpinBox.activeFocus
                                && !maxIntervalSpinBox.up.pressed
                                && !maxIntervalSpinBox.down.pressed
                        }

                        onValueModified: Revise.SettingsService.maxInterval = value
                    }

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: qsTr("Максимальный интервал повторения какой-либо карточки")
                    }
                }

                // learning speed
                Revise.SettingsRow {
                    text: qsTr("Скорость обучения")

                    Row {
                        width: 180
                        spacing: 8

                        Revise.Text {
                            id: learningSpeedText
                            text: learningSpeedSlider.value.toFixed(1)
                            anchors.verticalCenter: learningSpeedSlider.verticalCenter
                        }

                        Revise.Slider {
                            id: learningSpeedSlider

                            width: parent.width - learningSpeedText.width - parent.spacing
                            from: 1.0
                            to: 5.0

                            Binding on value {
                                value: Revise.SettingsService.learningRate
                                when: !learningSpeedSlider.pressed
                            }

                            onMoved: Revise.SettingsService.learningRate = value
                        }
                    }

                    Revise.HoverableTooltip {
                        anchors.fill: parent
                        text: qsTr("Насколько быстро будет меняться сложность карточек")
                    }
                }
            }

            // Build info
            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/build.svg"
                header.label.text: qsTr("Информация о сборке")

                Revise.KeyValue {
                    key.text: qsTr("Имя компилятора")
                    value.text: Revise.BuildInfo.compilerId
                }

                Revise.KeyValue {
                    key.text: qsTr("Версия компилятора")
                    value.text: Revise.BuildInfo.compilerVersion
                }

                Revise.KeyValue {
                    key.text: qsTr("Целевая архитектура")
                    value.text: Revise.BuildInfo.targetArch
                }

                Revise.KeyValue {
                    key.text: qsTr("Имя хост-системы")
                    value.text: Revise.BuildInfo.buildHostSystem
                }

                Revise.KeyValue {
                    key.text: qsTr("Хост-архитектура")
                    value.text: Revise.BuildInfo.hostArch
                }

                Revise.KeyValue {
                    key.text: qsTr("Тип сборки")
                    value.text: Revise.BuildInfo.buildType
                }

                Revise.KeyValue {
                    key.text: qsTr("Флаги компилятора")
                    value.text: Revise.BuildInfo.buildFlags
                }

                Revise.KeyValue {
                    key.text: qsTr("Версия Qt")
                    value.text: Revise.BuildInfo.buildQtVersion
                }

                Revise.KeyValue {
                    key.text: qsTr("Хэш коммита в Git")
                    value.text: Revise.BuildInfo.buildGitCommitHash
                }
            }
        }
    }

    function onEnter() {}
    function onExit() {}
}
