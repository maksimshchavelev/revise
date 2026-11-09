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

                // UI scale
                Revise.SettingsRow {
                    text: qsTr("Масштаб интерфейса")

                    Row {
                        spacing: 8

                        Revise.ValueSlider {
                            id: uiScaleSlider

                            label.text: `${value.toFixed(0)}%`
                            slider.from: 70
                            slider.to: 130
                            slider.stepSize: 5
                            value: Revise.SettingsService.uiScale * 100
                        }

                        Revise.Button {
                            text: qsTr("Применить")
                            onClicked: Revise.SettingsService.uiScale = uiScaleSlider.value / 100
                        }
                    }
                }

                // Show notification about early access
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Уведомлять о раннем доступе")
                    checked: Revise.SettingsService.notifyAboutEarlyAccess
                    onChanged: function(checked) {
                        Revise.SettingsService.notifyAboutEarlyAccess = checked
                    }
                }
            }

            Revise.SettingsPanel {
                width: parent.width
                header.icon.source: "qrc:/res/img/dialog.svg"
                header.label.text: qsTr("Диалоговые окна")

                // Ask about removing one card
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать удаление карточки")
                    checked: Revise.SettingsService.askRemoveOneCard
                    onChanged: function(checked) {
                        Revise.SettingsService.askRemoveOneCard = checked
                    }
                }

                // Ask about removing many cards
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать удаление нескольких карточек")
                    checked: Revise.SettingsService.askRemoveManyCards
                    onChanged: function(checked) {
                        Revise.SettingsService.askRemoveManyCards = checked
                    }
                }

                // Ask about relearning one card
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать сброс прогресса обучения для одной карточки")
                    checked: Revise.SettingsService.askRelearnOneCard
                    onChanged: function(checked) {
                        Revise.SettingsService.askRelearnOneCard = checked
                    }
                }

                // Ask about relearning many cards
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать сброс прогресса обучения для нескольких карточек")
                    checked: Revise.SettingsService.askRelearnManyCards
                    onChanged: function(checked) {
                        Revise.SettingsService.askRelearnManyCards = checked
                    }
                }

                // Ask about removing a deck
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать удаление колоды")
                    checked: Revise.SettingsService.askRemoveDeck
                    onChanged: function(checked) {
                        Revise.SettingsService.askRemoveDeck = checked
                    }
                }

                // Ask about set difficulty to many cards
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать пакетное изменение сложности карточек")
                    checked: Revise.SettingsService.askSetDifficultyForManyCards
                    onChanged: function(checked) {
                        Revise.SettingsService.askSetDifficultyForManyCards = checked
                    }
                }

                // Ask about set state to many cards
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать пакетное изменение статуса карточек")
                    checked: Revise.SettingsService.askSetStateForManyCards
                    onChanged: function(checked) {
                        Revise.SettingsService.askSetStateForManyCards = checked
                    }
                }

                // Ask about aborting learning
                Revise.LabeledCheckBox {
                    width: parent.width
                    text: qsTr("Подтверждать прерывание обучения")
                    checked: Revise.SettingsService.askAbortLearning
                    onChanged: function(checked) {
                        Revise.SettingsService.askAbortLearning = checked
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
                        from: 1
                        to: 365

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
                        text: qsTr("Максимальный интервал повторения какой-либо карточки от 1 до 365")
                    }
                }

                // learning speed
                Revise.SettingsRow {
                    text: qsTr("Скорость обучения")

                    Revise.ValueSlider {
                        id: learningRateSlider

                        label.text: value.toFixed(1)
                        slider.from: 1.0
                        slider.to: 5.0
                        slider.stepSize: 0.1

                        Binding on value {
                            value: Revise.SettingsService.learningRate
                            when: !learningRateSlider.pressed
                        }

                        onMoved: Revise.SettingsService.learningRate = value
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

                Revise.CopyableKeyValue {
                    key.text: qsTr("Название компилятора")
                    value.text: Revise.BuildInfo.compilerId
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Версия компилятора")
                    value.text: Revise.BuildInfo.compilerVersion
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Целевая архитектура")
                    value.text: Revise.BuildInfo.targetArch
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Имя хост-системы")
                    value.text: Revise.BuildInfo.buildHostSystem
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Хост-архитектура")
                    value.text: Revise.BuildInfo.hostArch
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Тип сборки")
                    value.text: Revise.BuildInfo.buildType
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Версия Qt")
                    value.text: Revise.BuildInfo.buildQtVersion
                }

                Revise.CopyableKeyValue {
                    key.text: qsTr("Хэш коммита в Git")
                    value.text: Revise.BuildInfo.buildGitCommitHash
                }
            }
        }
    }

    function onEnter() {}
    function onExit() {}
}
