import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Skywriter.Text
import Skywriter.Progress
import Skywriter.Theming
import "qrc:/qml/controls" as Sky
import "qrc:/qml/controls/dialog" as Sky
import "qrc:/js/dates.js" as Dates

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
            id: lastSavedLabel
            text: document.fileExists ? qsTr('Last saved %1').arg(Dates.relative(document.lastModified)) : ''
            color: ThemeManager.activeTheme.uiColor
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: 10
            Layout.alignment: Qt.AlignLeft
            Layout.preferredWidth: 400

            MouseArea {
                id: labelMouseArea
                anchors.fill: parent
                hoverEnabled: true
            }

            Sky.Tooltip {
                parent: parent
                visible: labelMouseArea.containsMouse
                text: document.lastModified.toLocaleString(Qt.locale());
            }

            Timer {
                interval: 60000
                running: document.fileExists
                repeat: true
                onTriggered: {
                    const relativeDate = Dates.relative(document.lastModified);

                    if (relativeDate !== lastSavedLabel.text) {
                        lastSavedLabel.text = Qt.binding(() => document.fileExists ? qsTr('Last saved %1').arg(Dates.relative(document.lastModified)) : '');
                    }
                }
            }
        }

        Label {
            text: qsTr('%L1 added today').arg(ProgressTracker.progressToday)
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
                text: qsTr('%L1 characters').arg(document.selectedText ? document.selectedCharacterCount : document.characterCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%L1 paragraphs').arg(document.selectedText ? document.selectedParagraphCount : document.paragraphCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%L1 pages').arg(document.selectedText ? document.selectedPageCount : document.pageCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%L1 words').arg(document.selectedText ? document.selectedWordCount : document.wordCount)
                color: ThemeManager.activeTheme.uiColor
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }
        }
    }
}
