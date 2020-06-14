import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.14

import 'qrc:/qml/effects' as Sky
import Skywriter.Theming 1.0

Rectangle {
    id: root
    property Theme theme: Theme {}
    property bool showUI: false
    default property list<Item> content
    clip: true
    layer.enabled: true
    layer.effect: Sky.CornerRadius { radius: 15 }
    color: theme.windowBackground

    function getRelativeWidth() {
        if (theme.documentWidth <= 1.0) {
            return width * theme.documentWidth;
        }

        return Math.min(width, theme.documentWidth / Screen.width * width);
    }

    function getRelativeHeight() {
        if (theme.documentHeight <= 1.0) {
            return height * theme.documentHeight;
        }

        return Math.min(height, theme.documentHeight / Screen.height * height);
    }

    Image {
        anchors.fill: parent
        fillMode: theme.fillMode
        source: theme.backgroundImage
        visible: status === Image.Ready
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: getRelativeWidth()
            Layout.preferredHeight: getRelativeHeight() - toolbar.height
            color: theme.documentBackground
            children: root.content
        }

        Rectangle {
            id: toolbar
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            visible: root.showUI
            Layout.preferredHeight: Math.max(0.05 * root.height, 20)
            color: theme.uiBackground

            Rectangle {
                anchors.verticalCenter: toolbar.verticalCenter
                anchors.left: toolbar.left
                anchors.leftMargin: 10
                radius: 10

                width: toolbar.width * 0.2
                height: 2
                color: theme.uiColor
            }

            Rectangle {
                anchors.verticalCenter: toolbar.verticalCenter
                anchors.horizontalCenter: toolbar.horizontalCenter
                radius: 10

                width: toolbar.width * 0.05
                height: 2
                color: theme.uiColor
            }

            Rectangle {
                anchors.verticalCenter: toolbar.verticalCenter
                anchors.right: toolbar.right
                anchors.rightMargin: 10
                radius: 10

                width: toolbar.width * 0.2
                height: 2
                color: theme.uiColor
            }
        }
    }
}
