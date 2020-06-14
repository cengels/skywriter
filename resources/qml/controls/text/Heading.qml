import QtQuick 2.14
import "qrc:/qml/controls/text" as Sky

Column {
    id: root
    property int level: 2
    property alias text: textControl.text
    property alias font: textControl.font
    property alias horizontalAlignment: textControl.horizontalAlignment
    property alias verticalAlignment: textControl.verticalAlignment
//    property alias topPadding: textControl.topPadding
//    property alias bottomPadding: textControl.bottomPadding
//    property alias leftPadding: textControl.leftPadding
//    property alias rightPadding: textControl.rightPadding
    bottomPadding: 10
    spacing: 7

    Sky.Text {
        id: textControl
        anchors.left: root.left
        anchors.right: root.right
        anchors.leftMargin: 20
        color: palette.brightText
        font.letterSpacing: 2
        font.pointSize: {
            switch (level) {
                case 1: return 15;
                case 2: return 14;
                case 3: return 13;
                case 4: return 12;
                case 5: return 11;
                case 6:
                default: return 10;
            }
        }
    }

    Rectangle {
        color: Qt.darker(palette.text, 1.5)
        height: 1
        anchors.left: root.left
        anchors.right: root.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
    }
}
