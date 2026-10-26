// SpinBox.qml

import QtQuick
import Revise as Revise

/**
 * \qmltype SpinBox
 * \brief A numeric input text field that restricts entries to integers within a configurable range.
 *
 * This control extends \c Revise.TextField to accept only digital input. It uses an
 * internal \c IntValidator bound to the \c minimum and \c maximum properties to enforce
 * value constraints.
 *
 * \b {Example Usage:}
 * \code
 * SpinBox {
 *     width: 120
 *     minimum: 1
 *     maximum: 3600
 *     text: "60"
 * }
 * \endcode
 */
Revise.TextField {
    id: root

    width: 120
    text: root.value

    /**
     * \qmlproperty int SpinBox::minimum
     * The minimum acceptable integer value for the validator.
     * The default value is \c 0.
     */
    property int minimum: 0

    /**
     * \qmlproperty int SpinBox::maximum
     * The maximum acceptable integer value for the validator.
     * The default value is \c 100.
     */
    property int maximum: 100

    /**
     * \qmlproperty int SpinBox::value
     * The value of SpinBox.
     * The default value is \c 0.
     */
    property int value: 0

    inputMethodHints: Qt.ImhDigitsOnly
    validator: IntValidator {
        bottom: root.minimum
        top: root.maximum
    }

    onTextChanged: root.value = parseInt(text)
}
