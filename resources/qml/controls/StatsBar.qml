import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Skywriter.Text 1.0

ToolBar {
    property FormattableTextArea document;

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Label {
            text: 'Last saved at [...]'
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            font.pointSize: 10
            Layout.alignment: Qt.AlignLeft
            Layout.preferredWidth: 400
        }

        Label {
            text: '0 added today'
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
                text: document.wordCount + ' words'
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: document.pageCount + ' pages'
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: document.paragraphCount + ' paragraphs'
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }

            Label {
                text: document.characterCount + ' characters'
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pointSize: 10
            }
        }
    }
}
