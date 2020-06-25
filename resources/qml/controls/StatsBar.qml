import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Skywriter.Text 1.0
import Skywriter.Progress 1.0
import Skywriter.Theming 1.0
import "qrc:/qml/controls" as Sky

Sky.CollapsiblePane {
    horizontalPadding: 7
    verticalPadding: 2
    property FormattableTextArea document;

    RowLayout {
        id: rowLayout
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        Label {
            text: document.fileExists ? qsTr('Last saved %1').arg(Qt.formatDateTime(document.lastModified)) : ''
            color: ThemeManager.activeTheme.uiColor
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: 10
            Layout.alignment: Qt.AlignLeft
            Layout.preferredWidth: 400
        }

        Label {
            text: qsTr('%1 added today').arg(ProgressTracker.progressToday)
            color: ThemeManager.activeTheme.uiColor
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: 10
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }

        Row {
            Layout.alignment: Qt.AlignRight
            layoutDirection: Qt.RightToLeft
            spacing: 25
            Layout.preferredWidth: 400

            Label {
                text: qsTr('%1 characters').arg(document.selectedText ? document.selectedCharacterCount : document.characterCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 paragraphs').arg(document.selectedText ? document.selectedParagraphCount : document.paragraphCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 pages').arg(document.selectedText ? document.selectedPageCount : document.pageCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 words').arg(document.selectedText ? document.selectedWordCount : document.wordCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }
        }
    }
}
