// MobileAddDeckDelegate.qml
// Mobile implementation of AddDeckDelegate

import QtQuick
import QtQuick.Layouts
import Revise as Revise

/*!
    \qmltype MobileAddDeckDelegate
    \brief A mobile delegate for creating or importing a new deck.
    \inqmlmodule Revise
    \inherits Item
    \since 1.0

    MobileAddDeckDelegate provides a horizontally stretched interface for the deck list,
    allowing the user to either create a new deck or import an existing one.
    It includes a header, a description, and two action buttons stacked vertically.

    The component supports a loading state that fades out the content and
    displays a spinner in the center.

    This component is typically used internally by \l AddDeckDelegate and is not meant
    to be instantiated directly.

    \section1 Example Usage

    \qml
    Revise.MobileAddDeckDelegate {
        onCreateClicked: openDeckEditor()
        onImportClicked: openImportDialog()
    }
    \endqml
*/
Item {
    id: root

    width: parent.width
    height: 140

    /*!
        \qmlproperty bool MobileAddDeckDelegate::loading
        \brief Whether the delegate is in a loading state.

        When \c true, the main content fades out and a spinner is displayed
        in the center.

        The default value is \c false.
    */
    property bool loading: false

    /*!
        \qmlproperty string MobileAddDeckDelegate::header
        \brief The title text displayed at the top of the delegate.

        The default value is \c qsTr("Добавить колоду").
    */
    property string header: qsTr("Добавить колоду")

    /*!
        \qmlproperty string MobileAddDeckDelegate::description
        \brief The descriptive text explaining the available actions.

        The default value is \c qsTr("Вы можете либо создать новую колоду, либо импортировать существующую в формате Anki или Revise").
    */
    property string description: qsTr("Вы можете либо создать новую колоду, либо импортировать существующую в формате Anki или Revise")

    /*!
        \qmlsignal MobileAddDeckDelegate::importClicked()
        \brief Emitted when the "Import" button is clicked.
    */
        signal importClicked

    /*!
        \qmlsignal MobileAddDeckDelegate::createClicked()
        \brief Emitted when the "Create" button is clicked.
    */
        signal createClicked

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

        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }

        anchors {
            fill: parent
            margins: 12
        }

        // Header
        Revise.Text {
            Layout.fillWidth: true
            text: root.header
            elide: Text.ElideRight
            color: Revise.Theme.foregroundColor
            font.bold: true
        }

        Revise.VerticalSpacer {}

        Row {
            Layout.fillWidth: true
            spacing: 6

            Revise.Text {
                id: txt
                width: 150
                text: root.description
                elide: Text.ElideRight
                color: Revise.Theme.secondaryForegroundColor
                font.pixelSize: Revise.Theme.fontSizeSmall
                wrapMode: Text.WordWrap
            }

            Column {
                width: parent.width - txt.width - spacing
                spacing: 6

                // Create
                Revise.Button {
                    width: parent.width
                    height: 30

                    text: qsTr("Создать")
                    font.bold: true
                    onClicked: root.createClicked()
                }

                // Import
                Revise.Button {
                    width: parent.width
                    height: 30

                    text: qsTr("Импортировать")
                    font.bold: true
                    onClicked: root.importClicked()
                }
            }
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
}
