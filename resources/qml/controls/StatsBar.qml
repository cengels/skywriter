import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Skywriter.Text 1.0
import Skywriter.Progress 1.0

ToolBar {
    property FormattableTextArea document;

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Label {
            text: qsTr('Last saved %1').arg(Qt.formatDateTime(document.lastModified))
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: 10
            Layout.alignment: Qt.AlignLeft
            Layout.preferredWidth: 400
        }

        Label {
            text: qsTr('%1 added today').arg(ProgressTracker.progressToday)
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
                text: qsTr('%1 characters').arg(document.characterCount)
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 paragraphs').arg(document.paragraphCount)
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 pages').arg(document.pageCount)
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: qsTr('%1 words').arg(document.wordCount)
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }
        }
    }
}
