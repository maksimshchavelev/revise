// DesktopAddDeckDelegate.qml
// Desktop implementation of AddDeckDelegate

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype DesktopAddDeckDelegate
    \brief A desktop delegate for creating or importing a new deck.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    DesktopAddDeckDelegate provides a card-like interface for the deck list,
    allowing the user to either create a new deck or import an existing one.
    It includes a header, a description, and two action buttons.

    The component supports a loading state that fades out the content and
    displays a spinner, and a subtle hover animation for visual feedback.

    This component is typically used internally by \l AddDeckDelegate and is not meant
    to be instantiated directly.

    \section1 Example Usage

    \qml
    Revise.DesktopAddDeckDelegate {
        onCreateClicked: openDeckEditor()
        onImportClicked: openImportDialog()
    }
    \endqml
*/
Item {
    id: root

    width: 170
    height: 220

    /*!
        \qmlproperty bool DesktopAddDeckDelegate::loading
        \brief Whether the delegate is in a loading state.

        When \c true, the main content fades out and a spinner is displayed
        in the center.

        The default value is \c false.
    */
    property bool loading: false

    /*!
        \qmlproperty string DesktopAddDeckDelegate::header
        \brief The title text displayed at the top of the delegate.

        The default value is \c qsTr("Добавить колоду").
    */
    property string header: qsTr("Добавить колоду")

    /*!
        \qmlproperty string DesktopAddDeckDelegate::description
        \brief The descriptive text explaining the available actions.

        The default value is \c qsTr("Вы можете создать новую колоду или импортировать существующую из Anki или Revise").
    */
    property string description: qsTr("Вы можете создать новую колоду или импортировать существующую из Anki или Revise")

    /*!
        \qmlsignal DesktopAddDeckDelegate::importClicked()
        \brief Emitted when the "Import" button is clicked.
    */
        signal importClicked

    /*!
        \qmlsignal DesktopAddDeckDelegate::createClicked()
        \brief Emitted when the "Create" button is clicked.
    */
        signal createClicked

    scale: hoverHandler.hovered ? 1.02 : 1.0

    Rectangle {
        id: tint
        anchors.fill: parent
        color: Revise.Theme.surfaceColor
        radius: 10
        layer.effect: Revise.Shadow {}
    }

    ColumnLayout {
        id: content
        visible: opacity > 0
        opacity: root.loading ? 0.0 : 1.0
        spacing: 8

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }

        anchors {
            fill: parent
            margins: 8
        }

        // Header
        Revise.Text {
            Layout.fillWidth: true
            text: root.header
            elide: Text.ElideRight
            color: Revise.Theme.foregroundColor
            font.bold: true
        }

        Revise.Spacer {}

        // Description
        Revise.Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: root.description
            elide: Text.ElideRight
            color: Revise.Theme.secondaryForegroundColor
            font.pixelSize: Revise.Theme.fontSizeSmall
            wrapMode: Text.WordWrap
        }

        // Create
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30

            text: qsTr("Создать")
            font.bold: true
            onClicked: root.createClicked()
        }

        // Import
        Revise.Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: parent.width * 0.9
            Layout.preferredHeight: 30
            Layout.bottomMargin: 4

            text: qsTr("Импортировать")
            font.bold: true
            onClicked: root.importClicked()
        }
    }

    Revise.Spinner {
        id: spinner
        visible: opacity > 0
        opacity: root.loading ? 1.0 : 0.0
        running: true
        size: 36
        anchors.centerIn: parent

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 60
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
