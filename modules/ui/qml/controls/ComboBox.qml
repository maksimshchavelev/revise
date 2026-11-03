// ComboBox.qml

import QtQuick
import QtQuick.Controls
import Revise as Revise

/*!
    \qmltype ComboBox
    \brief A drop-down list styled for the Revise design system.
    \inqmlmodule Revise
    \inherits ComboBox
    \since 1.0

    ComboBox is a combined button and popup list styled to match the Revise
    design system. It provides a compact control that displays the currently
    selected item in a closed state and presents the full list of options in
    an animated popup when opened.

    The control supports per-item text colors through the \c textColor role
    of the model. If a model item does not define \c textColor, the item is
    rendered using \l {Revise.Theme::foregroundColor}{Revise.Theme.foregroundColor}.
    This allows a single model to mix uniformly colored items with items that
    override the color individually.

    The popup uses a scale and opacity animation on both opening and closing,
    growing from the top edge of the control so the motion is anchored to the
    field the user interacted with.

    The control automatically selects the first item when a non-empty model
    is assigned or populated, so consumers do not need to set
    \l {ComboBox::currentIndex}{currentIndex} manually.

    \section1 Example Usage

    A combo box with a static model where each item defines its own text
    color:

    \qml
    Revise.ComboBox {
        model: ListModel {
            Component.onCompleted: {
                append({
                    label: "New",
                    textColor: Revise.Theme.green
                })
                append({
                    label: "Review",
                    textColor: Revise.Theme.blue
                })
                append({
                    label: "Incorrect",
                    textColor: Revise.Theme.red
                })
            }
        }
    }
    \endqml

    Items without a \c textColor role fall back to the default foreground
    color:

    \qml
    Revise.ComboBox {
        model: ListModel {
            ListElement { label: "Default" }
            ListElement { label: "Highlighted"; textColor: "#FF6B6B" }
        }
    }
    \endqml

    \sa ComboBox
*/
ComboBox {
    id: root

    implicitWidth: 160
    implicitHeight: 30

    textRole: "label"

    background: Rectangle {
        color: Revise.Theme.surfaceContainerColor
        radius: 6
    }

    delegate: ItemDelegate {
        id: delegateItem

        required property string label
        required property var textColor

        width: ListView.view.width - 16
        hoverEnabled: true

        background: Rectangle {
            color: delegateItem.hovered ? "white" : "transparent"
            opacity: 0.05
            radius: 6
        }

        contentItem: Revise.Text
        {
            text: delegateItem.label
            maximumLineCount: 3
            elide: Revise.Text.ElideRight
            color: (delegateItem.textColor === undefined || delegateItem.textColor === null)
                ? Revise.Theme.foregroundColor
                : delegateItem.textColor
        }
    }

    contentItem: Revise.Text
    {
        anchors.fill: parent
        anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: root.displayText
        maximumLineCount: 1
        elide: Revise.Text.ElideRight
        color: {
            if (root.currentIndex < 0 || root.count === 0)
                return Revise.Theme.foregroundColor
            const c = root.model.get(root.currentIndex).textColor
            return (c === undefined || c === null) ? Revise.Theme.foregroundColor : c
        }
    }

    popup: Popup {
        y: root.height
        width: root.width
        implicitHeight: contentItem.implicitHeight
        transformOrigin: Item.Top

        contentItem: ListView {
            clip: true
            spacing: 2
            implicitHeight: contentHeight
            model: root.delegateModel
            currentIndex: root.highlightedIndex
            boundsBehavior: ListView.StopAtBounds
            ScrollBar.vertical: Revise.Scrollbar {
                backgroundItem.color: Color.lighter(Revise.Theme.surfaceContainerColor, 1.2)
                sliderItem.color: Revise.Theme.primaryColor
            }
        }

        background: Rectangle {
            color: Revise.Theme.surfaceContainerColor
            radius: 6
            border.width: 1
            border.color: Color.lighter(Revise.Theme.surfaceContainerColor, 1.2)
            layer.enabled: true
            layer.effect: Revise.Shadow{}
        }

        enter: Transition {
            NumberAnimation {
                property: "opacity";
                from: 0;
                to: 1; duration: 60
            }
            NumberAnimation {
                property: "scale"
                from: 0.92
                to: 1.0
                duration: 180
                easing.type: Easing.OutBack
            }
        }

        exit: Transition {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 120
                easing.type: Easing.InQuad
            }
            NumberAnimation {
                property: "scale"
                from: 1.0
                to: 0.92
                duration: 140
                easing.type: Easing.InBack
            }
        }
    }

    onCountChanged: {
        if (root.currentIndex < 0 && root.count > 0)
            root.currentIndex = 0
    }

    onModelChanged: {
        if (root.currentIndex < 0 && root.count > 0)
            root.currentIndex = 0
    }

    Component.onCompleted: {
        if (root.currentIndex < 0 && root.count > 0)
            root.currentIndex = 0
    }
}
