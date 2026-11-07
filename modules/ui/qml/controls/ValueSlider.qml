// ValueSlider.qml

import QtQuick
import Revise as Revise

/*!
    \qmltype ValueSlider
    \brief A slider with its current value shown next to it.
    \inqmlmodule Revise
    \inherits Row
    \since 1.0

    ValueSlider pairs a \l Revise.Slider with a text label that displays
    the slider's current value. The label sits on the left and the slider
    takes the remaining width.

    The component exposes the label and the slider as aliases, so their
    properties can be adjusted from the outside, and forwards the slider's
    \c moved signal as \l moved.

    \section1 Example Usage

    \qml
    Revise.ValueSlider {
        value: Revise.SettingsService.learningRate
        onMoved: Revise.SettingsService.learningRate = value
    }
    \endqml
*/
Row {
    id: root

    width: 180
    spacing: 8

    /*!
        \qmlproperty Revise.Text ValueSlider::label
        \brief The text element that displays the current value.

        The text is bound to \l value and rounded to the nearest integer.
        Its width is fixed at 30 pixels.
    */
    property alias label: label

    /*!
        \qmlproperty Revise.Slider ValueSlider::slider
        \brief The slider controlled by the component.

        The slider spans from \c 0 to \c 100 and takes the width that
        remains after the label and \l {Row::spacing}{spacing}.
    */
    property alias slider: slider

    /*!
        \qmlproperty real ValueSlider::value
        \brief The current value of the slider.

        This property is an alias to
        \l {Revise.Slider::value}{slider.value}.
    */
    property alias value: slider.value

    /*!
        \qmlproperty bool ValueSlider::pressed
        \brief Whether the slider is currently pressed.

        This property is read-only and reflects
        \l {Revise.Slider::pressed}{slider.pressed}.
    */
    readonly property bool pressed: root.slider.pressed

    /*!
        \qmlsignal ValueSlider::moved()
        \brief Emitted when the user moves the slider.

        This signal is forwarded from
        \l {Revise.Slider::moved}{Revise.Slider.moved} and is not emitted
        when \l value is changed programmatically.
    */
    signal moved()

    Revise.Text {
        id: label
        width: 30
        text: slider.value.toFixed(0)
        anchors.verticalCenter: slider.verticalCenter
    }

    Revise.Slider {
        id: slider
        width: root.width - label.width - root.spacing
        from: 0
        to: 100

        onMoved: root.moved()
    }
}
